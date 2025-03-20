#include <LiquidCrystal.h>

// Initialize the library with the numbers of the interface pins
LiquidCrystal lcd(8, 7, 6, 5, 4, 3);

// Define custom characters for Ä, Ö, and Å
byte A_umlaut[8] = { 
  0b01010,  // Umlaut dots
  0b00000,  
  0b01110,  //  ***
  0b10001,  // *   *
  0b11111,  // *****
  0b10001,  // *   *
  0b10001,  // *   *
  0b00000   
};

byte O_umlaut[8] = { 
  0b01010,  // Umlaut dots
  0b00000,  
  0b01110,  //  ***
  0b10001,  // *   *
  0b10001,  // *   *
  0b10001,  // *   *
  0b01110,  //  ***
  0b00000   
};

byte A_ring[8] = {  
  0b00100,  //  *
  0b01010,  // * *
  0b00100,  //  *
  0b01110,  //  ***
  0b10001,  // *   *
  0b11111,  // *****
  0b10001,  // *   *
  0b00000   
};


void setup() {
  lcd.begin(16, 2); // Set up the LCD with 16 columns and 2 rows
  lcd.createChar(0, A_umlaut);
  lcd.createChar(1, O_umlaut);
  lcd.createChar(2, A_ring);
  lcd.clear(); 
}

void loop() {
  char letters[] = { 'A', 'B', 'C', 'D', 'E', 'F', 'G', 'H', 'I', 'J', 'K', 'L', 'M', 'N', 'P', 'Q', 'R', 'S', 'T', 'U', 'V', 'W', 'X', 'Y', 'Z', 2, 0, 1 };
  int index = 0;

  while (true) {
    // Move letter across the top row from left to right
    for (int col = 0; col < 16; col++) {
      lcd.clear();
      lcd.setCursor(col, 0);
      lcd.write(letters[index]);
      index = (index + 1) % (sizeof(letters) / sizeof(letters[0]));
      delay(300);
    }

    // Move letter across the bottom row from right to left
    for (int col = 15; col >= 0; col--) {
      lcd.clear();
      lcd.setCursor(col, 1);
      lcd.write(letters[index]);
      index = (index + 1) % (sizeof(letters) / sizeof(letters[0]));
      delay(300);
    }
  }
}
