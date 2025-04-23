#include <LiquidCrystal.h>

const byte windPin = 2;  // D2 pin
volatile unsigned long pulseTimestamps[10]; // Stores timestamps of 10 pulses in an array
volatile byte pulseIndex = 0; // Keeps track of pulse index (used with pulseTimestamps[])
volatile bool enoughData = false; // Turns true when 10 pulses are detected

LiquidCrystal lcd(8, 7, 6, 5, 4, 3); // The LCD screen pins where the circuit is connected to

void setup() {
  lcd.begin(16, 2);
  pinMode(windPin, INPUT);
  attachInterrupt(digitalPinToInterrupt(windPin), onPulse, FALLING);
  lcd.print("MITATAAN DATAA");
}

void loop() {
  if (enoughData) {
    noInterrupts(); // Disable interrupts while reading shared data
    unsigned long times[10];
    for (byte i = 0; i < 10; i++) {
      times[i] = pulseTimestamps[i];
    }
    interrupts(); // Enable interrupts again

    // Calculate the average period of 9 intervals between 10 pulses
    float totalInterval = 0;
    for (byte i = 1; i < 10; i++) {
      totalInterval += (times[i] - times[i - 1]);
    }

    float avgPeriod = totalInterval / 9.0; // in milliseconds
    float frequency = 1000.0 / avgPeriod;  // Hz
    float windSpeed = -0.24 + frequency * 0.699;

    if (windSpeed < 0) windSpeed = 0;

    lcd.clear();
    lcd.setCursor(0, 0);
    lcd.print("F: ");
    lcd.print(frequency, 2);
    lcd.print(" Hz");

    lcd.setCursor(0, 1);
    lcd.print("U: ");
    lcd.print(windSpeed, 2);
    lcd.print(" m/s");

    pulseIndex = 0;
    enoughData = false;
  }
}

void onPulse() {
  unsigned long now = millis();
  pulseTimestamps[pulseIndex] = now;
  pulseIndex++;

  if (pulseIndex >= 10) {
    enoughData = true;
  }
}