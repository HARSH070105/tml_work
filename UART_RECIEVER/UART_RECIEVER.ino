// ESP32-B: UART2 ← ESP32-A → Serial Monitor

#define UART_TX 17  // TX2 (not used here)
#define UART_RX 16  // RX2 pin

void setup() {
  Serial.begin(115200);            // USB serial for output
  Serial2.begin(9600, SERIAL_8N1, UART_RX, UART_TX);  // UART2
  Serial.println("Waiting for number from UART2...");
}

void loop() {
  if (Serial2.available()) {
    String received = Serial2.readStringUntil('\n');
    received.trim();
    Serial.print("Received via UART2: ");
    Serial.println(received);
  }
}
