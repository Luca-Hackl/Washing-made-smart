//Libraries
#include <SPI.h>//https://www.arduino.cc/en/reference/SPI
#include <MFRC522.h>//https://github.com/miguelbalboa/rfid
#include <elapsedMillis.h>

//Constants
#define SS_PIN 5
#define RST_PIN 15

int pin = 21;

const int delayBetweenMeasure = 600000; // 10 minutes
const int measureDuration = 60000; // 1 minute


byte tomsTag[4] = {103, 97, 67, 25};
byte myTag2[4] = {103, 97, 67, 25};


//Variables
byte nuidPICC[4] = {0, 0, 0, 0};
MFRC522::MIFARE_Key key;
MFRC522 rfid = MFRC522(SS_PIN, RST_PIN);

void setup() {

   pinMode(pin, INPUT); //initializes sensor

  //Init Serial USB
  Serial.begin(115200);
  Serial.println(F("Initialize System"));

  //init rfid D8,D5,D6,D7
  SPI.begin();
  rfid.PCD_Init();

  Serial.print(F("Reader :"));
  rfid.PCD_DumpVersionToSerial();
}

void loop() {
  readRFID();
}

void readRFID(void) { /* function readRFID */
  
  // Scan for RFID card
  for (byte i = 0; i < 6; i++) {
    key.keyByte[i] = 0xFF;
  }

  // Return if no card is present
  if (!rfid.PICC_IsNewCardPresent())
    return;

  // Verify if the NUID has been read
  if (!rfid.PICC_ReadCardSerial())
    return;

  // Store NUID into nuidPICC array
  for (byte i = 0; i < 4; i++) {
    nuidPICC[i] = rfid.uid.uidByte[i];
  }

  Serial.print(F("Read the following tag: "));
  printDec(rfid.uid.uidByte, rfid.uid.size);
  Serial.println();

    for (byte i = 0; i < 4; i++) {
        if (nuidPICC[i] != tomsTag[i]){
       } 

    }

  // Halt PICC
  rfid.PICC_HaltA();

  // Stop encryption on PCD
  rfid.PCD_StopCrypto1();

  washingAnalysisLoop();

}

void washingAnalysisLoop(){
  boolean isRunning = true;
  Serial.println("Washing loop startet");

  while(isRunning == true){
    Serial.println("Waiting for " + String(delayBetweenMeasure) + " milliseconds");
    delay(delayBetweenMeasure);
    Serial.println("Starting measurement");

    int measure = 0;
    for (int i = 0; i < measureDuration; i++) {
      measure += digitalRead(pin);
      delay(1);
    }
    if (measure == 0){
        isRunning = false;
        sendMessage();
    }
  }
}

/**
   Helper routine to dump a byte array as hex values to Serial.
*/
void printHex(byte *buffer, byte bufferSize) {
  for (byte i = 0; i < bufferSize; i++) {
    Serial.print(buffer[i] < 0x10 ? " 0" : " ");
    Serial.print(buffer[i], HEX);
  }
}

/**
   Helper routine to dump a byte array as dec values to Serial.
*/
void printDec(byte *buffer, byte bufferSize) {
  for (byte i = 0; i < bufferSize; i++) {
    Serial.print(buffer[i] < 0x10 ? " 0" : " ");
    Serial.print(buffer[i], DEC);
  }
}

void sendMessage(){
  Serial.println("Washing maschine finished!");
}
