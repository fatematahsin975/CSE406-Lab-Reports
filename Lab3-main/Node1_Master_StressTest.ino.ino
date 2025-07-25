// #include <SoftwareSerial.h> // REMOVE OR COMMENT OUT THIS LINE

// Define Hardware Serial (UART0) pins implicitly by using 'Serial' object.
// GPIO1 (TX) and GPIO3 (RX) are used by default.
// No SoftwareSerial instance needed.

// Test parameters
const int baudRates[] = {115200}; // ONLY testing 115200 baud rate
const int messageSizes[] = {10, 50, 100};
const int intervals[] = {0, 10, 100}; // Delay in ms
const unsigned long testDuration = 10000; // 10 seconds per test
const int timeout = 500; // 500ms timeout for response

void setup() {
  Serial.begin(115200); // This initializes Hardware Serial (UART0) for initial debug output
  Serial.println("NodeMCU 1: UART Master Stress Test (Hardware Serial @ 115200 Baud Only)");
}

void loop() {
  // Run tests for each baud rate (now only 115200)
  for (int baud : baudRates) {
    Serial.begin(baud); // Re-initialize Hardware Serial for communication at the new baud rate
    Serial.print("Testing baud rate: "); // This message might not appear on monitor if baud changes immediately
    Serial.println(baud); // Same as above

    // Send baud rate change command to slave via Hardware Serial
    Serial.print("BAUD:");
    Serial.println(baud);
    delay(500); // Allow NodeMCU 2 to sync its baud rate before sending data

    // Run tests for each message size
    for (int size : messageSizes) {
      // Run tests for each interval
      for (int interval : intervals) {
        runTest(baud, size, interval);
        delay(2000); // Pause between different test permutations
      }
    }
    // No Serial.end() needed here as we want the hardware serial to remain active.
    // If you wanted to restore debug, you'd do Serial.begin(115200); here.
  }
  Serial.println("All 115200 baud tests complete. Restarting in 10 seconds...");
  delay(10000); // Wait before repeating all tests
}

void runTest(int baud, int size, int interval) {
  // These print statements will go to the Hardware Serial port, not USB monitor during test
  // unless the baud rate for the USB monitor matches the current test baud.
  Serial.print("Test: Baud=");
  Serial.print(baud);
  Serial.print(", Size=");
  Serial.print(size);
  Serial.print(" bytes, Interval=");
  Serial.print(interval);
  Serial.println("ms");

  unsigned long startTime = millis();
  int messagesSent = 0;
  int messagesReceived = 0;
  int errors = 0;
  unsigned long bytesSent = 0;
  int sequence = 0;

  // Generate test message
  String baseMessage = "D";
  while (baseMessage.length() < size - 6) { // Reserve 6 bytes for sequence number and colon
    baseMessage += "X";
  }

  // Run test for the specified duration (10 seconds)
  while (millis() - startTime < testDuration) {
    // Send message with sequence number
    String message = String(sequence) + ":" + baseMessage;
    Serial.println(message); // Send message over Hardware Serial
    bytesSent += message.length() + 2; // Include \r\n (carriage return and newline)
    messagesSent++;
    sequence++;

    // Wait for response from slave with timeout
    unsigned long responseStart = millis();
    String receivedData = "";
    bool received = false;

    while (millis() - responseStart < timeout) {
      if (Serial.available()) {
        char c = Serial.read();
        if (c == '\n') { // Message ends with newline
          received = true;
          break;
        }
        receivedData += c;
      }
    }

    // Trim received data to remove potential \r or other whitespace
    receivedData.trim();

    // Verify response
    if (received && receivedData == message) {
      messagesReceived++;
    } else {
      errors++;
      if (!received) {
        // This output goes to the serial line, likely not your USB monitor directly
        Serial.println("Timeout error: No response received within " + String(timeout) + "ms.");
      } else {
        // This output goes to the serial line, likely not your USB monitor directly
        Serial.print("Mismatch error: Expected '");
        Serial.print(message);
        Serial.print("', Got '");
        Serial.print(receivedData);
        Serial.println("'");
      }
    }

    // Delay for specified interval between messages
    if (interval > 0) {
      delay(interval);
    }
  }

  // Calculate and print test results
  float durationSeconds = testDuration / 1000.0;
  float throughput = bytesSent / durationSeconds; // Bytes per second
  float messageRate = messagesSent / durationSeconds; // Messages per second
  float errorRate = (messagesSent > 0) ? (errors * 100.0) / messagesSent : 0.0; // Percentage, handle division by zero

  // These results will be printed to the Hardware Serial line
  Serial.println("Test Results:");
  Serial.print("Messages Sent: ");
  Serial.println(messagesSent);
  Serial.print("Messages Received: ");
  Serial.println(messagesReceived);
  Serial.print("Errors: ");
  Serial.println(errors);
  Serial.print("Error Rate: ");
  Serial.print(errorRate);
  Serial.println("%");
  Serial.print("Throughput: ");
  Serial.print(throughput);
  Serial.println(" bytes/second");
  Serial.print("Message Rate: ");
  Serial.print(messageRate);
  Serial.println(" messages/second");
  Serial.println();
}