import cv2
import numpy as np
import serial
import time
from picamera2 import Picamera2

rois = []
drawing = False
ref_point = []

DETECTION_PARAMS = {
    "winStride": (5, 4),
    "padding": (16, 16),
    "scale": 1.05,
    "hitThreshold": -0.2,
}

def draw_roi(event, x, y, flags, param):
    global ref_point, drawing, rois, frame

    if event == cv2.EVENT_LBUTTONDOWN:
        ref_point = [(x, y)]
        drawing = True

    elif event == cv2.EVENT_MOUSEMOVE and drawing:
        frame_copy = frame.copy()
        cv2.rectangle(frame_copy, ref_point[0], (x, y), (0, 255, 0), 2)
        cv2.imshow("Human Detector - ROI Selection", frame_copy)

    elif event == cv2.EVENT_LBUTTONUP:
        ref_point.append((x, y))
        drawing = False
        x1, y1 = ref_point[0]
        x2, y2 = ref_point[1]
        rois.append((min(x1, x2), min(y1, y2), max(x1, x2), max(y1, y2)))
        print(f"ROI added: {rois[-1]}")

def main():
    global frame

    # --- UART Serial Setup ---
    try:
        ser = serial.Serial('/dev/serial0', 115200, timeout=1)
        time.sleep(2)
        print("Serial connection established.")
    except Exception as e:
        print(f"Warning: Could not open serial port. {e}")
        ser = None

    # --- Human Detector Setup ---
    hog = cv2.HOGDescriptor()
    hog.setSVMDetector(cv2.HOGDescriptor_getDefaultPeopleDetector())

    # --- Initialize Pi Camera 2 ---
    picam2 = Picamera2()
    picam2.preview_configuration.main.size = (640, 480)
    picam2.preview_configuration.main.format = "RGB888"
    picam2.configure("preview")
    picam2.start()
    time.sleep(1)

    window_name = "Human Detector - ROI Selection"
    cv2.namedWindow(window_name)
    cv2.setMouseCallback(window_name, draw_roi)

    detection_mode = False

    print("--- ROI Selection Mode ---")
    print("Press 'd' to draw a new box.")
    print("Press 'c' to clear all boxes.")
    print("Press 's' to start detection.")
    print("Press 'q' to quit.")

    while True:
        frame = picam2.capture_array()

        key = cv2.waitKey(1) & 0xFF
        if key == ord('q'):
            break
        elif key == ord('s') and not detection_mode:
            if not rois:
                print("Warning: No ROIs defined.")
            else:
                detection_mode = True
                print("\n--- Detection Mode Started ---")
                cv2.destroyWindow(window_name)
                window_name = "Human Detector - Running"
                cv2.namedWindow(window_name)
        elif key == ord('c'):
            rois.clear()
            print("All ROIs cleared.")
        elif key == ord('d'):
            print("Ready to draw.")

        if not detection_mode:
            display_frame = frame.copy()
            cv2.putText(display_frame, "Mode: ROI Selection", (10, 30),
                        cv2.FONT_HERSHEY_SIMPLEX, 0.7, (0, 0, 255), 2)
            for (x1, y1, x2, y2) in rois:
                cv2.rectangle(display_frame, (x1, y1), (x2, y2), (0, 255, 0), 2)
            cv2.imshow(window_name, display_frame)

        else:
            display_frame = frame.copy()
            cv2.putText(display_frame, "Mode: Detection | q: Quit", (10, 30),
                        cv2.FONT_HERSHEY_SIMPLEX, 0.7, (0, 0, 255), 2)
            status_report = []

            for i, (x1, y1, x2, y2) in enumerate(rois):
                roi_frame = frame[y1:y2, x1:x2]
                if roi_frame.size == 0:
                    continue

                detections, _ = hog.detectMultiScale(roi_frame, **DETECTION_PARAMS)
                roi_has_human = len(detections) > 0
                roi_color = (0, 255, 0) if roi_has_human else (0, 0, 255)

                status_report.append(f"ROI #{i+1}: {'DETECTED' if roi_has_human else 'Clear'}")
                cv2.rectangle(display_frame, (x1, y1), (x2, y2), roi_color, 2)

                for (x, y, w, h) in detections:
                    abs_x, abs_y = x + x1, y + y1
                    cv2.rectangle(display_frame, (abs_x, abs_y),
                                  (abs_x + w, abs_y + h), (255, 0, 0), 2)

                if roi_has_human and ser:
                    message = f"DETECTED in Frame {i+1}\n"
                    ser.write(message.encode('utf-8'))

            print(' | '.join(status_report), end='\r')
            cv2.imshow(window_name, display_frame)

    picam2.stop()
    if ser:
        ser.close()
        print("\nSerial connection closed.")
    cv2.destroyAllWindows()
    print("\nApplication closed.")

if __name__ == '__main__':
    main()
