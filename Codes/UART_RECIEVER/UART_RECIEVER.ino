#include <Arduino.h>

// UART Pins
#define UART_TX 17
#define UART_RX 16

// Servo PWM GPIOs
#define VENT1_SERVO_PIN 32  // Control for Frame 1
#define VENT2_SERVO_PIN 25  // Control for Frame 2

// Servo parameters for MG996R
const int pwmFreq = 50;              // 50Hz
const int pwmResolution = 16;        // 16-bit resolution
const int maxDuty = 65535;           // 2^16 - 1

// Servo angles
const int VENT_OPEN_ANGLE = 220;      // Open at 90°
const int VENT_CLOSE_ANGLE = 0;      // Closed at 0°

// Timing variables
unsigned long lastReceivedTime1 = 0;
unsigned long lastReceivedTime2 = 0;

//Default State of vents
bool vent1Closed = false;
bool vent2Closed = false;

// Debug flag
bool debugMode = true;

void setup() {
  Serial.begin(115200);
  delay(1000); // Give serial time to initialize
  
  Serial.println("=== ESP32 Servo Controller Starting ===");
  
  // Initialize UART2
  Serial2.begin(9600, SERIAL_8N1, UART_RX, UART_TX);
  Serial.println("UART2 initialized on pins RX:16, TX:17");

  // PWM channel setup with new API (ESP32 Arduino Core v3.x)
  if (!ledcAttach(VENT1_SERVO_PIN, pwmFreq, pwmResolution)) {
    Serial.println("ERROR: Failed to setup PWM for Servo 1");
  } else {
    Serial.println("PWM for Servo 1 setup successful");
  }

  if (!ledcAttach(VENT2_SERVO_PIN, pwmFreq, pwmResolution)) {
    Serial.println("ERROR: Failed to setup PWM for Servo 2");
  } else {
    Serial.println("PWM for Servo 2 setup successful");
  }

  // Initialize vents as open with delay
  Serial.println("Initializing servos to OPEN position...");
  moveServo(VENT1_SERVO_PIN, VENT_OPEN_ANGLE);
  delay(1000); // Give servo time to move
  moveServo(VENT2_SERVO_PIN, VENT_OPEN_ANGLE);
  delay(1000);

  // Initialize timing
  lastReceivedTime1 = millis();
  lastReceivedTime2 = millis();

  Serial.println("=== Setup Complete - Waiting for UART2 data ===");
  Serial.println("Expected format: 'Frame 1' or 'Frame 2'");
  Serial.println("Timeout: Frame 1 = 15s, Frame 2 = 15s");
}

void loop() {
  // Check for UART data
  if (Serial2.available()) {
    String received = Serial2.readStringUntil('\n');
    received.trim();

    if (received.length() > 0) {
      if (debugMode) {
        Serial.print("[UART] Received: '");
        Serial.print(received);
        Serial.println("'");
      }

      // Check for Frame 1
      if (received.indexOf("Frame 1") != -1) {
        lastReceivedTime1 = millis();
        Serial.println("[FRAME1] Signal detected - Opening vent 1");
        
        if (vent1Closed) {
          moveServo(VENT1_SERVO_PIN, VENT_OPEN_ANGLE);
          vent1Closed = false;
          Serial.println("[FRAME1] Vent 1 opened");
        } else {
          Serial.println("[FRAME1] Vent 1 already open");
        }
      } 
      // Check for Frame 2
      else if (received.indexOf("Frame 2") != -1) {
        lastReceivedTime2 = millis();
        Serial.println("[FRAME2] Signal detected - Opening vent 2");
        
        if (vent2Closed) {
          moveServo(VENT2_SERVO_PIN, VENT_OPEN_ANGLE);
          vent2Closed = false;
          Serial.println("[FRAME2] Vent 2 opened");
        } else {
          Serial.println("[FRAME2] Vent 2 already open");
        }
      }
      else {
        Serial.print("[WARNING] Unknown message: ");
        Serial.println(received);
      }
    }
  }

  // Timeout logic for Frame 1 (15 seconds) Change 15000 to desired time in ms
  if (!vent1Closed && (millis() - lastReceivedTime1 > 15000)) {
    Serial.println("[TIMEOUT] Frame 1 timeout - Closing vent 1");
    moveServo(VENT1_SERVO_PIN, VENT_CLOSE_ANGLE);
    vent1Closed = true;
  }

  // Timeout logic for Frame 2 (15 seconds) Change 15000 to desired time in ms
  if (!vent2Closed && (millis() - lastReceivedTime2 > 15000)) {
    Serial.println("[TIMEOUT] Frame 2 timeout - Closing vent 2");
    moveServo(VENT2_SERVO_PIN, VENT_CLOSE_ANGLE);
    vent2Closed = true;
  }

  // Debug status every 10 seconds
  static unsigned long lastDebugTime = 0;
  if (debugMode && (millis() - lastDebugTime > 10000)) {
    printStatus();
    lastDebugTime = millis();
  }

  // Check for serial commands for testing
  if (Serial.available()) {
    String command = Serial.readStringUntil('\n');
    command.trim();
    command.toLowerCase();
    
    if (command == "test1") {
      Serial.println("[TEST] Testing Servo 1");
      moveServo(VENT1_SERVO_PIN, VENT_CLOSE_ANGLE);
      delay(1000);
      moveServo(VENT1_SERVO_PIN, VENT_OPEN_ANGLE);
    }
    else if (command == "test2") {
      Serial.println("[TEST] Testing Servo 2");
      moveServo(VENT2_SERVO_PIN, VENT_CLOSE_ANGLE);
      delay(1000);
      moveServo(VENT2_SERVO_PIN, VENT_OPEN_ANGLE);
    }
    else if (command == "status") {
      printStatus();
    }
    else if (command == "debug") {
      debugMode = !debugMode;
      Serial.print("[DEBUG] Debug mode: ");
      Serial.println(debugMode ? "ON" : "OFF");
    }
  }

  delay(50); // Small delay to prevent excessive CPU usage
}

// Improved servo control function for MG996R
void moveServo(int pin, int angle) {
  // Constrain angle
  angle = constrain(angle, 0, 180);
  
  // Calculate duty cycle for MG996R (1ms-2ms pulse width)
  // For 16-bit resolution at 50Hz:
  // 1ms = 3277, 1.5ms = 4915, 2ms = 6553
  int minDuty = 3277;  // 1ms pulse width (0 degrees)
  int maxDuty = 6553;  // 2ms pulse width (180 degrees)
  
  int duty = map(angle, 0, 180, minDuty, maxDuty);
  
  ledcWrite(pin, duty);
  
  // Debug output
  Serial.print("[SERVO] Pin ");
  Serial.print(pin);
  Serial.print(" set to ");
  Serial.print(angle);
  Serial.print("° (duty: ");
  Serial.print(duty);
  Serial.println(")");
}

// Status printing function
void printStatus() {
  Serial.println("=== STATUS ===");
  Serial.print("Vent 1: ");
  Serial.print(vent1Closed ? "CLOSED" : "OPEN");
  Serial.print(" | Last signal: ");
  Serial.print((millis() - lastReceivedTime1) / 1000);
  Serial.println("s ago");
  
  Serial.print("Vent 2: ");
  Serial.print(vent2Closed ? "CLOSED" : "OPEN");
  Serial.print(" | Last signal: ");
  Serial.print((millis() - lastReceivedTime2) / 1000);
  Serial.println("s ago");
  
  Serial.print("Free heap: ");
  Serial.print(ESP.getFreeHeap());
  Serial.println(" bytes");
  Serial.println("=============");
}