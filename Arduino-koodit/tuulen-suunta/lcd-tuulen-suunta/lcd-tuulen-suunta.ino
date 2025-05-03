#include <LiquidCrystal.h>

const int sensorPin = A5; // Käytetään A5 analogilukemaan
const int numSamples = 10; // 10 näytettä
int readings[numSamples]; // Tallennetaan 10 arvoa
int sampleIndex = 0;
unsigned long lastSampleTime = 0;
const unsigned long sampleInterval = 1000; // 1 sekunti

LiquidCrystal lcd(8, 7, 6, 5, 4, 3);

void setup() {
  Serial.begin(9600);
  lcd.begin(16, 2);
  pinMode(sensorPin, INPUT);
  lcd.print("MITATAAN DATAA");
}

void loop() {
  unsigned long currentTime = millis();

  if (currentTime - lastSampleTime >= sampleInterval && sampleIndex < numSamples) {
    lastSampleTime = currentTime;

    int value = analogRead(sensorPin);
    readings[sampleIndex] = value;
    sampleIndex++;

    Serial.print("Sample ");
    Serial.print(sampleIndex);
    Serial.print(": ");
    Serial.println(value);
  }

  // Kun kaikki 10 näytettä on kerätty, lasketaan keskiarvo ja näytetään se
  if (sampleIndex >= numSamples) {
    int sum = 0;
    for (int i = 0; i < numSamples; i++) {
      sum += readings[i];
    }
    float average = sum / (float)numSamples;
    float voltage = average * (5.0 / 1023.0);
    int degree = map((int)average, 0, 773, 0, 360);

    Serial.print("Average analog value: ");
    Serial.println(average);
    Serial.print("Average voltage: ");
    Serial.print(voltage);
    Serial.println(" V");
    Serial.print("Average degree: ");
    Serial.print(degree);
    Serial.println(" deg");

    lcd.clear();
    lcd.setCursor(0, 0);
    lcd.print("TUULEN SUUNTA");
    lcd.setCursor(0, 1);
    lcd.print(degree);
    lcd.print(" deg");

    // Resetoi seuraavaa 10s sykliä varten
    sampleIndex = 0;
  }
}