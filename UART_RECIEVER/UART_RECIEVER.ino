#define UART_TX 17
#define UART_RX 16

#define VENT1_GPIO 32  // For Frame 1 (output-capable)
#define VENT2_GPIO 25  // For Frame 2 (instead of GPIO 34)

unsigned long lastReceivedTime1 = 0;
unsigned long lastReceivedTime2 = 0;
bool vent1Closed = false;
bool vent2Closed = false;

void setup() {
  Serial.begin(115200);
  Serial2.begin(9600, SERIAL_8N1, UART_RX, UART_TX);

  pinMode(VENT1_GPIO, OUTPUT);
  pinMode(VENT2_GPIO, OUTPUT);
  digitalWrite(VENT1_GPIO, HIGH);  // Default HIGH
  digitalWrite(VENT2_GPIO, HIGH);  // Default HIGH

  Serial.println("Waiting for detection from UART2...");
}

void loop() {
  // Handle incoming UART data
  if (Serial2.available()) {
    String received = Serial2.readStringUntil('\n');
    received.trim();

    if (received.length() > 0) {
      Serial.print("Received via UART2: ");
      Serial.println(received);

      // Identify which frame was detected
      if (received.indexOf("Frame 1") != -1) {
        lastReceivedTime1 = millis();

        if (vent1Closed) {
          digitalWrite(VENT1_GPIO, HIGH);
          vent1Closed = false;
        }
      } else if (received.indexOf("Frame 2") != -1) {
        lastReceivedTime2 = millis();

        if (vent2Closed) {
          digitalWrite(VENT2_GPIO, HIGH);
          vent2Closed = false;
        }
      }
    }
  }

  // Check timeout for Frame 1
  if (!vent1Closed && millis() - lastReceivedTime1 > 150000) {
    Serial.println("Vent 1 closed");
    digitalWrite(VENT1_GPIO, LOW);
    vent1Closed = true;
  }

  // Check timeout for Frame 2
  if (!vent2Closed && millis() - lastReceivedTime2 > 15000) {
    Serial.println("Vent 2 closed");
    digitalWrite(VENT2_GPIO, LOW);
    vent2Closed = true;
  }
}
