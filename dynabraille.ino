/*
  Dynabraille Interpreter
  Author: Katherine Perez
*/
//Pin connected to ST_CP of 74HC595
int latchPin = 19;
//Pin connected to SH_CP of 74HC595
int clockPin = 5;
////Pin connected to DS of 74HC595
int dataPin = 2;

byte data;
byte g1Braille[127];
byte digit;

int incomingByte = 0;

char buffer[4];
char character;

void setup() {
  //set pins to output because they are addressed in the main loop
  pinMode(latchPin, OUTPUT);
  Serial.begin(9600);

  //Binary notation as comment
  g1Braille[35] = 0x3C; //0b111100 - #
  g1Braille[49] = 0x01; //0b000001 - 1
  g1Braille[50] = 0x03; //0b011010 - 2
  g1Braille[51] = 0x09; //0b001001 - 3
  g1Braille[52] = 0x19; //0b011001 - 4
  g1Braille[53] = 0x11; //0b010001 - 5
  g1Braille[54] = 0x0B; //0b001001 - 6
  g1Braille[55] = 0x1B; //0b011011 - 7
  g1Braille[56] = 0x13; //0b010011 - 8
  g1Braille[57] = 0x0A; //0b001010 - 9
  g1Braille[58] = 0x1A; //0b011010 - 0
  g1Braille[65] = 0x01; //0b000001 - A
  g1Braille[66] = 0x03; //0b000011 - B
  g1Braille[67] = 0x09; //0b001001 - C
  g1Braille[68] = 0x19; //0b011001 - D
  g1Braille[69] = 0x11; //0b010001 - E
  g1Braille[70] = 0x0B; //0b001011 - F
  g1Braille[71] = 0x1B; //0b011011 - G
  g1Braille[72] = 0x13; //0b010011 - H
  g1Braille[73] = 0x0A; //0b001010 - I
  g1Braille[74] = 0x1A; //0b011010 - J
  g1Braille[75] = 0x05; //0b000101 - K
  g1Braille[76] = 0x07; //0b000111 - L
  g1Braille[77] = 0x0D; //0b001101 - M
  g1Braille[78] = 0x1D; //0b011101 - N
  g1Braille[79] = 0x15; //0b010101 - O
  g1Braille[80] = 0x0F; //0b001111 - P
  g1Braille[81] = 0x1F; //0b011111 - Q
  g1Braille[82] = 0x17; //0b010111 - R
  g1Braille[83] = 0x0E; //0b001110 - S
  g1Braille[84] = 0x1E; //0b011110 - T
  g1Braille[85] = 0x25; //0b100101 - U
  g1Braille[86] = 0x27; //0b100111 - V
  g1Braille[87] = 0x3A; //0b111010 - W
  g1Braille[88] = 0x2D; //0b101101 - X
  g1Braille[89] = 0x3D; //0b111101 - Y
  g1Braille[90] = 0x35; //0b110101 - Z
  g1Braille[97] = 0x01; //0b000001 - a
  g1Braille[98] = 0x03; //0b000011 - b
  g1Braille[99] = 0x09; //0b001001 - c
  g1Braille[100] = 0x19; //0b011001 - d
  g1Braille[101] = 0x11; //0b010001 - e
  g1Braille[102] = 0x0B; //0b001011 - f
  g1Braille[103] = 0x1B; //0b011011 - g
  g1Braille[104] = 0x13; //0b010011 - h
  g1Braille[105] = 0x0A; //0b001010 - i
  g1Braille[106] = 0x1A; //0b011010 - j
  g1Braille[107] = 0x05; //0b000101 - k
  g1Braille[108] = 0x07; //0b000111 - l
  g1Braille[109] = 0x0D; //0b001101 - m
  g1Braille[110] = 0x1D; //0b011101 - n
  g1Braille[111] = 0x15; //0b010101 - o
  g1Braille[112] = 0x0F; //0b001111 - p
  g1Braille[113] = 0x1F; //0b011111 - q
  g1Braille[114] = 0x17; //0b010111 - r
  g1Braille[115] = 0x0E; //0b001110 - s
  g1Braille[116] = 0x1E; //0b011110 - t
  g1Braille[117] = 0x25; //0b100101 - u
  g1Braille[118] = 0x27; //0b100111 - v
  g1Braille[119] = 0x3A; //0b111010 - w
  g1Braille[120] = 0x2D; //0b101101 - x
  g1Braille[121] = 0x3D; //0b111101 - y
  g1Braille[122] = 0x35; //0b110101 - z

}

void loop() {
  if (Serial.available() == 0) {
    Serial.print("Enter a 4-letter string: ");
    while (Serial.available() == 0) {
      
    }
  }

  if (Serial.available() > 0) {

    Serial.readBytes(buffer, 4);

    Serial.println(buffer);

    printBrailleLine(buffer, sizeof(buffer), g1Braille);

    for (int i = 3; i > -1 ; i--) {
      // load braille cell, last character first
      character = int(buffer[i]);
      data = g1Braille[buffer[i]];

      //printBrailleCell(character, g1Braille);
  
      digitalWrite(latchPin, 0);
  
      shiftOut(dataPin, clockPin, data);
  
      digitalWrite(latchPin, 1);

      //delay(500);

      buffer[i] = '\0';
  
    }  
  }
}

void shiftOut(int myDataPin, int myClockPin, byte myDataOut) {
  // This shifts 8 bits out MSB first,
  //on the rising edge of the clock,
  //clock idles low

  //internal function setup
  int i = 0;
  int pinState;
  pinMode(myClockPin, OUTPUT);
  pinMode(myDataPin, OUTPUT);

  //clear everything out just in case to
  //prepare shift register for bit shifting
  digitalWrite(myDataPin, 0);
  digitalWrite(myClockPin, 0);

  //for each bit in the byte myDataOutï¿½
  //NOTICE THAT WE ARE COUNTING DOWN in our for loop
  //This means that %00000001 or "1" will go through such
  //that it will be pin Q0 that lights.
  for (i = 7; i >= 0; i--)  {
    digitalWrite(myClockPin, 0);

    //if the value passed to myDataOut and a bitmask result
    // true then... so if we are at i=6 and our value is
    // %11010100 it would the code compares it to %01000000
    // and proceeds to set pinState to 1.
    if ( myDataOut & (1 << i) ) {
      pinState = 1;
    }
    else {
      pinState = 0;
    }

    //Sets the pin to HIGH or LOW depending on pinState
    digitalWrite(myDataPin, pinState);
    //register shifts bits on upstroke of clock pin
    digitalWrite(myClockPin, 1);
    //zero the data pin after shift to prevent bleed through
    digitalWrite(myDataPin, 0);
  }

  //stop shifting
  digitalWrite(myClockPin, 0);

}

void printBrailleCell(byte character, byte brailleArray[]) {
  // Print only one braille cell per line
  
  byte data = brailleArray[character];
  byte digit = data;
  int result[6];
  int printOrder[6] = {0, 3, 1, 4, 2, 5};

  char on = char(149),
       off = 'o';
  byte disp;
  
      for (int d = 0; d < 6; d++) {
        result[d] = digit % 2;
        digit = digit >> 1;
      }

      for (int i = 0; i < 6; i++) {
        disp = result[printOrder[i]];
        if (disp == 0) {
          Serial.print(off);
        }
        else if (disp == 1) {
          Serial.print(on);
        }
        if (i % 2){
          Serial.print("\n");
        }
      }
}

void printBrailleLine(char buffer[], int maxLength, byte brailleArray[]) {
  // Print all cells in a row

  int maxSize = maxLength*6;
  int maxColumnLength = maxLength*2;
  char character;
  byte data;
  int printOrder[6] = {0, 3, 1, 4, 2, 5};

  int result[maxSize];
  int finalPrintedResult[maxSize];

  char on = char(149),
       off = 'o';
  byte disp;

  int count = 0;
  while (count < maxSize) {
    for (int i = 0; i < maxLength; i++) {
      character = buffer[i];
      data = brailleArray[character];
      digit = data;
      for (int d = 0; d < 6; d++) {
          result[count] = digit % 2;
          digit = digit >> 1;
          count++;
      }
    }
  }


  Serial.print("\n");
  
  count = 0;
  int i = 0;
  while (count < maxSize) {
    finalPrintedResult[count] = result[i];
    i = i + 3;
    count++;
    if (count == maxColumnLength) {
      i = 1;
    }
    else if (count == maxColumnLength*2) {
      i = 2;
    }
  }

  count = 0;
  while (count < maxSize) {
    for (int rowCount = 0; rowCount < 3; rowCount++) {
      for (int columnCount = 0; columnCount < maxColumnLength; columnCount++) {
        disp = finalPrintedResult[count];
        if (disp == 0) {
          Serial.print(off);
        }
        else if (disp == 1) {
          Serial.print(on);
        }
        if (columnCount % 2) {
          Serial.print("   ");
        }
        count++;
      }
      Serial.print("\n");
    }
  }

  Serial.print("\n");
}

