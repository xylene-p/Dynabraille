#include <SPI.h>
#include <WiFi.h>
#include <PubNub.h>
#include <aJSON.h>

#define MAXLINESIZE 10

// Network Name
char ssid[] = "UHWireless";

const static char pubkey[] = "";
const static char subkey[] = "";
const static char channel[] = "TestChannel";

int g1Braille[MAXLINESIZE];
int g2Braille[MAXLINESIZE];

int lineLength = 10;
bool bits[32];

int clockPin = 5;
int dataPin = 2;

byte data;

void setup()
{
  pinMode(dataPin, OUTPUT);
  pinMode(clockPin, OUTPUT);
  Serial.begin(115200);
  Serial.println("Serial set up");

  if (WiFi.status() == WL_NO_SHIELD) {
    Serial.println("WiFi shield not present");
    while(true); // stop
  }

  int status;
  // attempt to connect to Wifi network:
  do {
    Serial.print("WiFi connecting to SSID: ");
    Serial.println(ssid);

    // Connect to the network. Uncomment whichever line is right for you:
    status = WiFi.begin(ssid); // open network
    //status = WiFi.begin(ssid, keyIndex, key); // WEP network
    //status = WiFi.begin(ssid, password); // WPA / WPA2 Personal network
  } while (status != WL_CONNECTED);
  Serial.println("WiFi set up");

  PubNub.begin(pubkey, subkey);
  Serial.println("PubNub set up");
}

void processMessagePacket(aJsonObject *item) {
  Serial.println("Processing message...");
  aJsonObject *root = aJson.getArrayItem(item, 0);
  if (!root) {
    Serial.println("no message data");
    return;
  }
  aJsonObject *pageData = getPageData(root);
  int array_count = aJson.getArraySize(pageData);
  aJsonObject *g1BrailleData = getBrailleData(pageData);
}

aJsonObject* getPageData(aJsonObject *root) {
  aJsonObject *pages = aJson.getObjectItem(root, "pages");
  return pages;
}

aJsonObject* getBrailleData(aJsonObject *pageData) {
  aJsonObject *brailleArray = aJson.getObjectItem(pageData, "g2Data");
  int maxSize = aJson.getArraySize(brailleArray);
  for (int i = 0; i < maxSize; i++) {
    aJsonObject *brailleCell = aJson.getArrayItem(brailleArray, i);
    g1Braille[i] = brailleCell->valueint;
    Serial.println(g1Braille[i]);
  }
}

// Braille Functions
void printBrailleLine(int maxLength, int brailleArray[]) {
  // Print all cells in a row

  int maxSize = maxLength*6;
  int maxColumnLength = maxLength*2;
  char character;
  int data, digit;
  int printOrder[6] = {0, 3, 1, 4, 2, 5};

  int result[maxSize];
  int finalPrintedResult[maxSize];

  char on = char(149),
       off = 'o';
  byte disp;

  int count = 0;
  while (count < maxSize) {
    for (int i = 0; i < maxLength; i++) {
      data = brailleArray[i];
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

void loop()
{
  WiFiClient *client;

  if (Serial.available() == 0) {
    Serial.println("===Press Enter to Refresh Braille Data===");
    while (Serial.available() == 0) {
      // wait for user input
    }
  }

  int input = Serial.read();
  Serial.println(input);

  if (input != 13) {
    return;
  }

  /* Retreive Last message */
  Serial.print("Retrieving last message from ");
  Serial.println(channel);
  client = PubNub.history(channel, 1);

  if (!client) {
    Serial.println("subscription error");
    delay(1000);
    return;
  }
  else {
    Serial.println("Message received");
  }

  /* Parse */

  aJsonClientStream stream(client);
  aJsonObject *msg = aJson.parse(&stream);
  client->stop();
  if (!msg) { Serial.println("parse error"); delay(1000); return; }
  processMessagePacket(msg);
  aJson.deleteItem(msg);

  printBrailleLine(lineLength, g1Braille);

  int brailleDisp[255];

  for (int i = 0; i < lineLength; i++) {
    brailleDisp[i] = g1Braille[i];
  }

  while (1) { // TODO: attach interrupts for user presence detection
    /* Send signals to braille pins */
    characterConcatenation(bits, brailleDisp);
    shiftReverse(bits, 32, 10);
  }

  delay(2500);
}

void characterConcatenation(bool *a_array, int *a_data)
{
    for (int i=0; i< lineLength; i++){
        for(int j=0; j<8; j++){
            a_array[(i*8)+j] = ((a_data[i] >> j) & 1);
        }
    }
}

void shiftReverse(bool *a_array, int a_size, int a_delay)
{
      for (int i=a_size-1; i>=0; i--){
        if (a_array[i] == 1){
            for(int j=a_size; j>=0; j--){
                if (j == i) shiftBit(1); // send out a 1
                else shiftBit(0);// send out a 0
            }
            delay(a_delay);
        }
    }
}

void shiftBit(bool a_bit)
{
    digitalWrite(dataPin, a_bit);   // send data to input pin
    digitalWrite(clockPin, HIGH);    // clock high to pass data
    digitalWrite(clockPin, LOW);    // return low to normal state
}
