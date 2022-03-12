//Libraries
#include <SPI.h>//https://www.arduino.cc/en/reference/SPI
#include <MFRC522.h>//https://github.com/miguelbalboa/rfid
#include <elapsedMillis.h>
#include <WiFi.h>
#include <HTTPClient.h>

//Constants
#define SS_PIN 5
#define RST_PIN 15

int pin = 21;

const int delayBetweenMeasure = 600000; // 10 minutes
const int measureDuration = 60000; // 1 minute


// mapping of a tag to a discord id
typedef struct { 
  byte tag[4];
  String discordID;
} nfcTagToDiscordID;

const nfcTagToDiscordID tagsToDiscord[] {
  {{115, 191, 129, 015}, "351460900370120707"}, // Tom
  {{147, 177, 217, 18}, "252061771181850625"} // Luca
};




const char* ssid = "Baumhaus";
const char* password = "9Oi1Fy_d+K666";
const char* webhookUrl = "https://discord.com/api/webhooks/950482190788083772/Vr1z0YAxqP6mapTyW3ihfcOSRROoiDNjX-JUsPBCtlUyv7L0k0GH7IThNH4yYCCEITdl";

HTTPClient http;

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

  // connecting to wifi
  WiFi.begin(ssid, password);
 
  Serial.println("Establishing connection to WiFi");
  while (WiFi.status() != WL_CONNECTED) {
    delay(1000);
    Serial.print(".");
  }
  Serial.print("\n");
  Serial.println("Connected to network with IP:");
  Serial.println(WiFi.localIP());

   sendDiscordMessage(
     "{\
       \"content\": null,\
       \"embeds\": [\
         {\
           \"title\": \"Washing analyzer started up\",\
           \"color\": 65280\
         }\
       ]\
     }"
   );
}

void loop() {
  readRFID();
}

void readRFID() { /* function readRFID */
  
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

  // Halt PICC
  rfid.PICC_HaltA();

  // Stop encryption on PCD
  rfid.PCD_StopCrypto1();

  washingAnalysisLoop();

}

void washingAnalysisLoop(){
  boolean isRunning = true;
  Serial.println("Washing loop startet");

  sendDiscordMessage(
     "{\
       \"content\": \"<@!" + tagToDiscordID(nuidPICC) + ">\", \
       \"embeds\": [\
         {\
           \"title\": \"Washing monitoring started\",\
           \"color\": 65280\
         }\
       ]\
     }"
   );

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
  Serial.println("Washing machine finished!");
  
  sendDiscordMessage(
     "{\
       \"content\": \"<@!" + tagToDiscordID(nuidPICC) + ">\", \
       \"embeds\": [\
         {\
           \"title\": \"Washing machine finished!\",\
           \"color\": 65280\
         }\
       ]\
     }"
   );
}

void sendDiscordMessage(String content) {
  http.begin(webhookUrl);
  http.addHeader("Content-Type", "application/json");
  http.POST(content);
}

String tagToDiscordID(byte tag[]) {
  for (uint8_t i = 0; i < sizeof(tagsToDiscord); i++)
  {
    bool equals = true;
    for (byte j = 0; j < 4; j++) {
        if (tag[j] != tagsToDiscord[i].tag[j]){
          equals = false;
        } 
    }
    if (equals) {
      return tagsToDiscord[i].discordID;
    }
  }
  return "";
}