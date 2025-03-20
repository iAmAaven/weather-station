#include <LiquidCrystal.h>

// Initialize the library with the numbers of the interface pins
const int sensorPin = A0;
LiquidCrystal lcd(8, 7, 6, 5, 4, 3);
void setup() {
  Serial.begin(9600);
  // Set up the LCD's number of columns and rows
  lcd.begin(16, 2);

  // Print "Hello World!" to the LCD
  pinMode(A0, INPUT);

  lcd.print("ARCADIA");
}

// the loop function runs over and over again forever
void loop() {
  int A1pin = analogRead(A1);
  float A1voltage = A1pin * (5.0 / 1023.0);

  Serial.print("Analog in: ");
  Serial.print(A1pin);
  Serial.print(", voltage: ");
  Serial.print(A1voltage);
  Serial.println(" V");
  delay(1000);

  int degree = map(A1pin, 0, 773, 0, 360);
  lcd.setCursor(0, 0);
  lcd.print("Wind direction");
  lcd.setCursor(0, 1);
  lcd.print("Degrees ");
  lcd.print(degree);
  lcd.print("  ");


  delay(2000);
  lcd.clear();
}