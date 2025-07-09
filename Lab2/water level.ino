// Define sensor input pins
const int sensorLow = 2;
const int sensorMedium = 3;
const int sensorHigh = 4;

// Define LED output pins
const int ledGreen = 8;
const int ledYellow = 9;
const int ledRed = 10;

void setup() {
  // Set sensor pins as input
  pinMode(sensorLow, INPUT);
  pinMode(sensorMedium, INPUT);
  pinMode(sensorHigh, INPUT);
  
  // Set LED pins as output
  pinMode(ledGreen, OUTPUT);
  pinMode(ledYellow, OUTPUT);
  pinMode(ledRed, OUTPUT);
}

void loop() {
  // Read water level sensor values
  bool low = digitalRead(sensorLow);
  bool medium = digitalRead(sensorMedium);
  bool high = digitalRead(sensorHigh);

  // Default: turn off all LEDs
  digitalWrite(ledGreen, LOW);
  digitalWrite(ledYellow, LOW);
  digitalWrite(ledRed, LOW);

  // Check water levels and light LEDs accordingly
  if (high) {
    digitalWrite(ledRed, HIGH); // High level
  } else if (medium) {
    digitalWrite(ledYellow, HIGH); // Medium level
  } else if (low) {
    digitalWrite(ledGreen, HIGH); // Low level
  }

  delay(500); // Update every half second
}
