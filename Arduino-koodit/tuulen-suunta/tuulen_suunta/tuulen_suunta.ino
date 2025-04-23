// Pin connected to the analog signal of the wind speed sensor
const int windSpeedPin = A0; // Adjust according to your wiring

// Digital output pin
const int digitalOutputPin = 7;

void setup() {
  Serial.begin(9600); // Start serial communication
  pinMode(digitalOutputPin, OUTPUT);
}

void loop() {
  int analogValue = analogRead(windSpeedPin); // Read the analog signal
  float voltage = analogValue * (5.0 / 1023.0); // Convert to voltage (0-5V)

  // Example threshold for digital conversion
  if (voltage > 2.5) {
    digitalWrite(digitalOutputPin, HIGH); // Wind speed above threshold
  } else {
    digitalWrite(digitalOutputPin, LOW); // Wind speed below threshold
  }

  // Output the readings to the serial monitor
  Serial.print("Analog value: ");
  Serial.print(analogValue);
  Serial.print(" | Voltage: ");
  Serial.println(voltage);

  delay(1000); // Delay for readability
} 
