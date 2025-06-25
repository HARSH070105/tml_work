import serial
import time
import random

# Configure your COM port and baud rate
COM_PORT = 'COM3'       # Change as per your system
BAUD_RATE = 115200      # Must match Serial.begin on ESP

try:
    ser = serial.Serial(COM_PORT, BAUD_RATE, timeout=1)
    print(f"Connected to {COM_PORT} at {BAUD_RATE} baud.")
    
    while True:
        number = random.randint(1, 5)
        ser.write(f"{number}\n".encode())  # Send number with newline
        print(f"Sent: {number}")
        time.sleep(10)  # Wait for 10 seconds

except serial.SerialException as e:
    print(f"Serial error: {e}")

except KeyboardInterrupt:
    print("Stopped by user.")

finally:
    if 'ser' in locals() and ser.is_open:
        ser.close()
        print("Serial connection closed.")