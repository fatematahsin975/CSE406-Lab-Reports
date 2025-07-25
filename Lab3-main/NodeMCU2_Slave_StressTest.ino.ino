#include <SoftwareSerial.h>

// Define SoftwareSerial pins
SoftwareSerial mySerial(D6, D5); // RX = D6, TX = D5

void setup() {
  Serial.begin(115200); // Debugging via USB Serial
  mySerial.begin(115200); // Initialize SoftwareSerial with a default baud rate
  Serial.println("NodeMCU 2: UART Slave Initialized");
}

void loop() {
  // Check for incoming data on SoftwareSerial (from Master)
  if (mySerial.available()) {
    String receivedLine = mySerial.readStringUntil('\n'); // Read until newline
    receivedLine.trim(); // Remove any leading/trailing whitespace, including '\r'

    // Check if the received message is a baud rate change command
    if (receivedLine.startsWith("BAUD:")) {
      int newBaud = receivedLine.substring(5).toInt(); // Extract baud rate
      if (newBaud > 0) { // Ensure a valid baud rate
        mySerial.end(); // Stop SoftwareSerial
        mySerial.begin(newBaud); // Start SoftwareSerial at the new baud rate
        Serial.print("NodeMCU 2: Switched to baud ");
        Serial.println(newBaud);
      } else {
        Serial.println("NodeMCU 2: Received invalid BAUD command: " + receivedLine);
      }
    } else {
      // If it's not a BAUD command, it's a regular data message
      Serial.print("NodeMCU 2: Received '");
      Serial.print(receivedLine);
      Serial.println("'");

      // Echo the message back to the Master
      mySerial.println(receivedLine);
      Serial.println("NodeMCU 2: Echoed message");
    }
  }
}