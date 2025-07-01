#define UART0_RX 3    // UART0 RX pin (receiving input)
#define UART0_TX 1    // UART0 TX pin (optional)
#define UART2_TX 17   // UART2 TX pin (to ESP32-B RX)
#define UART2_RX 16   // UART2 RX pin (not used here)

void setup() {
  Serial.begin(115200);   // USB serial for input
  Serial1.begin(115200, SERIAL_8N1, UART0_RX, UART0_TX);  // UART0
  Serial2.begin(115200, SERIAL_8N1, UART2_RX, UART2_TX);  // UART2
}

void loop() {
  String input = "";
  
  if (Serial.available()) {
    input = Serial.readStringUntil('\n');
    input.trim();
  }
  
  if (Serial1.available()) {
    input = Serial1.readStringUntil('\n');
    input.trim();
  }
  
  if (input.length() > 0) {
    Serial2.println(input);
    Serial.print("Forwarded: ");
    Serial.println(input);
  }
}