// Tämä ohjelma lukee tuulen nopeuden ja suunnan, näyttää tiedot LCD-näytöllä ja lähettää ne MQTT-palvelimelle.

#include <LiquidCrystal.h>
#include <Ethernet.h>
#include <PubSubClient.h>
#include <TimerOne.h>
#include <Keypad.h>

// LCD-näytön liitäntäpinnejä varten
const int rs = 8, e = 7, d4 = 6, d5 = 5, d6 = 4, d7 = 3;
LiquidCrystal lcd(rs, e, d4, d5, d6, d7);

// Aikakatkaisijan tila
bool timeswitch = true;

// Digitaalisen tuulimittarin tulo
const byte Din = 2;

// Näppäimistön määrittely (4x4 matriisi)
const byte ROWS = 4;
const byte COLS = 4;
byte rowPins[ROWS] = {18}; // Rivipinnit
byte colPins[COLS] = {14, 15, 16, 17}; // Sarakepinnit
char keys[ROWS][COLS] = {
    {'1', '2', '3', 'A'},
    {'4', '5', '6', 'B'},
    {'7', '8', '9', 'C'},
    {'*', '0', '#', 'D'}
};
Keypad customKeypad = Keypad(makeKeymap(keys), rowPins, colPins, ROWS, COLS);

// Ethernet- ja MQTT-määritykset
byte server[] = {10, 6, 0, 23}; // MQTT-palvelimen IP-osoite
unsigned int Port = 1883;       // MQTT-palvelimen portti
EthernetClient ethClient;       // Ethernet-kirjaston client-olio
PubSubClient client(server, Port, ethClient); // MQTT-client-olio
#define outTopic "ICT4_out_2020" // MQTT-lähetyksen aihe
static uint8_t mymac[6] = {0x44, 0x76, 0x59, 0x10, 0x00, 0x62}; // Ethernet MAC-osoite
char* clientId = "a731fsd4"; // MQTT-clientin tunniste

// Ajastimen määrittely
TimerOne messageTimer;

// setup-funktio, joka suoritetaan kerran ohjelman alussa
void setup() {
  lcd.begin(16, 2); // Määritetään LCD-näytön koko (16x2)
  Serial.begin(9600); // Sarjaliikenteen alustus
  Serial.println("Serial active"); // Ilmoitus sarjaliikenteen toiminnasta
  pinMode(Din, INPUT); // Määritetään digitaalinen tulo
  attachInterrupt(digitalPinToInterrupt(Din), takeTime, RISING); // Keskeytys tuulimittarin signaalille
  fetch_IP(); // Haetaan IP-osoite Ethernet-yhteydelle
  messageTimer.initialize(10000000); // Ajastimen asetus (10 sekuntia)
  messageTimer.attachInterrupt(timerISR); // Ajastimen keskeytyksen aktivointi
}

// Muuttujat keskiarvojen laskentaa ja muita toimintoja varten
volatile int digiSum = 0, digiCount = 0, degree;
volatile int analogSum = 0, analogCount = 0;
volatile bool sendFlag = true, printSwitch = true, ipData = false, minmax = false, digi = false, newEdge = false;
volatile int dmin = 0, dmax = 0, amin = 0, amax = 0;
volatile float frequency, windSpeed;
unsigned long timeold, timenew;

// Keskeytysrutiini tuulimittarin signaalin käsittelyyn
void takeTime() {
  timeold = timenew;
  timenew = micros(); // Tallennetaan nykyinen aika mikrosekunteina
  newEdge = true; // Merkitään uusi signaali havaittavaksi
  digiCount += 1; // Kasvatetaan digitaalisten mittausten laskuria
}

// Ajastimen keskeytysrutiini
void timerISR() {
  sendFlag = true; // Asetetaan lähetyslippu aktiiviseksi
}

// Ethernet-yhteyden muodostaminen ja IP-osoitteen hakeminen
void fetch_IP() {
    Serial.println("Starting ethernet connection");
    bool connectionSuccess = Ethernet.begin(mymac); // Yhdistetään Ethernet-verkkoon
    if (!connectionSuccess) {
        Serial.println("Failed to access Ethernet controller"); // Yhteys epäonnistui
    } else {
        Serial.println("Connected with IP: " + Ethernet.localIP()); // Tulostetaan IP-osoite
    }
}

// MQTT-palvelimeen yhdistäminen
void connect_MQTT_server() { 
    Serial.println("Connecting to MQTT");
    if (client.connect(clientId)) { // Tarkistetaan yhteys
        Serial.println("Connected OK"); // Yhdistetty onnistuneesti
    } else {
        Serial.println("Connection failed."); // Yhteys epäonnistui
    }    
}

// loop-funktio, joka suoritetaan jatkuvasti
void loop() {
  char key = customKeypad.getKey(); // Luetaan näppäimistön painallus
  if (key == '1') {
    printSwitch = false;
    ipData = true; // Näytetään IP-tiedot
    minmax = false;
  }
  else if(key == '2') {
    printSwitch = true; // Näytetään reaaliaikaiset tiedot
    ipData = false;
    minmax = false;
  }
  else if(key == '3') {
    printSwitch = false;
    ipData = false;
    minmax = true; // Näytetään minimi- ja maksimiarvot
    digi = !digi; // Vaihdetaan digitaalisen ja analogisen välillä
  }
  else if(key == 'A') {
    printSwitch = false;
    ipData = false;
    minmax = false;
    lcd.clear();
    lcd.setCursor(0, 0);
    lcd.print("Credits:");
    lcd.setCursor(0, 1);
    lcd.print("Tiimi Toivoton"); // Näytetään tiimin nimi
  }
  delay(100); // Lyhyt viive

  // Analogisen tuulimittarin lukeminen
  int A5pin = analogRead(A5);
  float A5voltage = A5pin * (5.0 / 1023.0); // Muutetaan jännite
  degree = map(A5pin, 0, 773, 0, 360); // Muutetaan asteiksi
  analogSum += degree;
  analogCount += 1;
  if(degree < amin) amin = degree; // Päivitetään minimiarvo
  if(degree > amax) amax = degree; // Päivitetään maksimiarvo

  // Digitaalisen tuulimittarin lukeminen
  if(newEdge) {
    unsigned long period = timenew - timeold;
    if(period > 0) {
      frequency = 1000000.0 / period; // Lasketaan taajuus
      windSpeed = round(-0.24 + frequency * 0.699); // Lasketaan tuulen nopeus
      digiSum += windSpeed;
    }
    Serial.println(frequency);
    Serial.println(windSpeed);
    newEdge = false;
  }
  
  if(windSpeed < dmin) dmin = windSpeed; // Päivitetään minimiarvo
  if(windSpeed > dmax) dmax = windSpeed; // Päivitetään maksimiarvo

  // Näytetään reaaliaikaiset tiedot LCD-näytöllä
  if(printSwitch) {
    lcd.clear();
    lcd.setCursor(0, 0);
    lcd.print("Wind ");
    lcd.print(windSpeed);
    lcd.print(" m/s");
    lcd.setCursor(0, 1);
    lcd.print("Dir ");
    lcd.print(degree);
    lcd.print(" Deg");
  }

  // Näytetään IP- ja MQTT-tiedot LCD-näytöllä
  if(ipData) {
    lcd.clear();
    lcd.setCursor(0, 0);
    lcd.print("10.6.0.23");
    lcd.setCursor(0, 1);
    if(client.connected()) {
      lcd.print("MQTT up");
    }
    else {
      lcd.print("MQTT down");
    }
  }
  
  // Näytetään minimi- ja maksimiarvot LCD-näytöllä
  if(minmax) {
    if(digi) {
      lcd.clear();
      lcd.setCursor(0, 0);
      lcd.print("Wind speed m/s");
      lcd.setCursor(0, 1);
      lcd.print("Min: ");
      lcd.print(dmin);
      lcd.print(" Max: ");
      lcd.print(dmax);
    }
    else {
      lcd.clear();
      lcd.setCursor(0, 0);
      lcd.print("Wind Dir (degrees)");
      lcd.setCursor(0, 1);
      lcd.print("Min: ");
      lcd.print(amin);
      lcd.print(" Max: ");
      lcd.print(amax);
    }
  }

  // Lähetetään tiedot MQTT-palvelimelle
  if (sendFlag) {
    if(!client.connected()) {
      connect_MQTT_server(); // Yhdistetään palvelimeen, jos ei ole yhteyttä
    }
    sendFlag = false;

    // Lähetetään analogiset tiedot
    int analogAvg = (analogCount > 0) ? analogSum / analogCount : 0;
    Serial.print("Sent analog = ");
    Serial.println(analogAvg);
    String msg = "IOTJS={\"S_name\":\"toivoton_TuulenSuunta\",\"S_value\":";
    msg.concat(analogAvg);
    msg.concat("}");
    int msg_len = msg.length() + 1;
    char payload[msg_len];
    msg.toCharArray(payload, msg_len);
    client.publish(outTopic, payload);
    delay(50);

    // Lähetetään digitaaliset tiedot
    int digiAvg = (digiCount > 0) ? digiSum / digiCount : 0;
    Serial.print("Sent digi = ");
    Serial.println(digiAvg);
    String msg1 = "IOTJS={\"S_name\":\"toivoton_TuulenNopeus\",\"S_value\":";
    msg1.concat(digiAvg);
    msg1.concat("}");
    int msg1_len = msg1.length() + 1;
    msg1.toCharArray(payload, msg1_len);
    client.publish(outTopic, payload);
    delay(50);
    Serial.println(payload);

    // Nollataan laskurit
    analogSum = 0;
    analogCount = 0;
    digiSum = 0;
    digiCount = 0;
  }
}