# Human Detection and Vent Control System for Commercial Vehicles

This project aims to develop an embedded system that uses a camera-based human detection module to control AC vents in commercial vehicles, enhancing energy efficiency and passenger comfort.

## 🗂️ Project Structure

```
📁 Codes
│   ├── Human Detection/                                # Code for detecting human presence using OpenCV + HOG
│   ├── Pi Zero 2 W/                                    # Setup and interfacing with Raspberry Pi Zero 2 W
│   ├── UART_RECIEVER/                                  # ESP32 code to receive UART signals and drive servos
│   └── UART_TRANSMITTER/                               # ESP32 code to transmit detected state to receiver
│
📁 Demonstration Videos
│   ├── Code Demo.mp4                                   # Walkthrough of the core code working
│   ├── Full Demo.mp4                                   # Complete system demo: detection, communication, vent control
│   └── Vent Mech.mp4                                   # Mechanical actuation of vents using servos
│
📁 Documentation and Spec Sheets
│   ├── Buck Converter.pdf                              # Power management circuit details
│   ├── MG996R.PDF                                      # Servo motor datasheet
│   ├── RPiCamMod2.pdf                                  # Raspberry Pi Camera Module 2 datasheet
│   ├── esp32_datasheet_en.pdf                          # ESP32 microcontroller datasheet
│   └── raspberry-pi-zero-2-w-product-brief.pdf         # Pi Zero 2 W specs
│
📄 Intern Report - Harsh Kapoor.pdf                    # Final internship report and project summary
```

## 💡 Project Overview

The system is designed to:

- Detect human presence inside the vehicle using computer vision techniques on Raspberry Pi.
- Transmit detection data via UART to an ESP32 microcontroller.
- Actuate AC vents using servo motors based on occupancy, saving energy when no one is present.

## 🛠️ Technologies Used

- **OpenCV + Python**: For real-time human detection.
- **ESP32**: For UART communication and servo control.
- **MG996R Servo**: For mechanical vent actuation.
- **Raspberry Pi Zero 2 W**: Low-power onboard computation.
- **UART Protocol**: For wired communication between nodes.
- **Buck Converter**: For power regulation and distribution.

## 🎥 Demo Videos

The [Demonstration Videos](./Demonstration%20Videos) folder contains:

- Working of the detection code
- Complete system integration
- Vent movement demo

## 📄 Documentation

Refer to the [Documentation and Spec Sheets](./Documentation%20and%20Spec%20Sheets) folder for:

- Datasheets of hardware components used
- Power circuit documentation

## 🧑‍💻 Developer

**Harsh Kapoor**  
Intern at ERC,
TATA MOTORS
July 2025

For further details, refer to the [Intern Report - Harsh Kapoor.pdf](./Intern%20Report%20-%20Harsh%20Kapoor.pdf)
