// ESP32-A: Serial Monitor → UART2 → ESP32-B

#define UART_TX 17  // TX2 pin
#define UART_RX 16  // RX2 pin (not used here)

void setup() {
  Serial.begin(115200);            // USB serial for input
  Serial2.begin(9600, SERIAL_8N1, UART_RX, UART_TX);  // UART2
  Serial.println("Enter a number:");
}

void loop() {
  if (Serial.available()) {
    String input = Serial.readStringUntil('\n');
    input.trim(); // remove newline or spaces
    Serial2.println(input); // Forward to UART2
    Serial.print("Forwarded: ");
    Serial.println(input);
  }
}
