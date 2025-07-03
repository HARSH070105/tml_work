#include <Arduino.h>

// UART2 Pins for forwarding data
#define UART2_TX 17
#define UART2_RX 16

// Status LED (optional)
#define STATUS_LED 2

// Communication parameters
const int USB_BAUD = 115200;    // USB Serial baud rate (matches Python)
const int UART2_BAUD = 9600;    // UART2 baud rate (matches receiver ESP32)

// Timing and status variables
unsigned long lastDataTime = 0;
unsigned long ledBlinkTime = 0;
bool ledState = false;
bool debugMode = true;
int messageCount = 0;

void setup() {
  // Initialize USB Serial (Serial0) - for receiving from Python
  Serial.begin(USB_BAUD);
  delay(1000); // Give serial time to initialize
  
  Serial2.begin(UART2_BAUD, SERIAL_8N1, UART2_RX, UART2_TX);
  Serial.println("UART2 initialized on pins TX:17, RX:16");
  
  // Initialize status LED
  pinMode(STATUS_LED, OUTPUT);
  digitalWrite(STATUS_LED, LOW);
}

void loop() {
  // Check for data from USB Serial (Python code)
  if (Serial.available()) {
    String receivedData = Serial.readStringUntil('\n');
    receivedData.trim();
    
    if (receivedData.length() > 0) {
      lastDataTime = millis();
      messageCount++;
      
      // Process and forward the message
      String forwardMessage = processMessage(receivedData);
      
      if (forwardMessage.length() > 0) {
        Serial2.println(forwardMessage);
      }
    }
  }
  
  delay(10); // Small delay to prevent excessive CPU usage
}

// Process incoming message and format for UART2
String processMessage(String input) {
  String output = "";
  
  // Look for detection messages from Python
  if (input.indexOf("DETECTED in Frame") != -1) {
    // Extract frame number
    int frameStart = input.indexOf("Frame ") + 6;
    int frameEnd = input.indexOf(" ", frameStart);
    if (frameEnd == -1) frameEnd = input.length();
    
    String frameNumber = input.substring(frameStart, frameEnd);
    frameNumber.trim();
    
    // Format message for receiver ESP32
    output = "Frame " + frameNumber;
    
    Serial.print("[PROCESS] Detection in Frame ");
    Serial.print(frameNumber);
    Serial.println(" - Forwarding to receiver");
    
  } else if (input.indexOf("Frame") != -1) {
    // Direct frame message - forward as is
    output = input;
    Serial.println("[PROCESS] Direct frame message - Forwarding");
    
  } else {
    // Unknown message format
    if (debugMode) {
      Serial.print("[PROCESS] Unknown format: ");
      Serial.println(input);
    }
  }
  
  return output;
}
