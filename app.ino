// ESP32S3-MFRC522-CUID

#include <SPI.h>
#include <MFRC522.h>

#define RST_PIN 4
#define SS_PIN 5
#define MISO_PIN 16
#define SCK_PIN 17
#define MOSI_PIN 18
#define XOR ^

MFRC522 rfid(SS_PIN, RST_PIN);
MFRC522::MIFARE_Key defaultKey;

// New UID (4 bytes) to write and BCC checksum (automatically calculated)
byte newUid[] = {0x12, 0x34, 0x56, 0x69}; // Replace with your UID

// Track card status
bool cardProcessed = false;
byte lastCardUID[4] = {0, 0, 0, 0};

void setup() {
  Serial.begin(9600);
  SPI.begin(SCK_PIN, MISO_PIN, MOSI_PIN, SS_PIN);
  rfid.PCD_Init();
  for (byte i = 0; i < 6; i++) {
      defaultKey.keyByte[i] = 0xFF;
  }
  delay(4);
  Serial.println("Ready to write UID to Gen2 card...");
}

void loop() {
  // Check if a card is present
  bool cardPresent = rfid.PICC_IsNewCardPresent();
  
  // If no card is present, reset processing status
  if (!cardPresent) {
    if (cardProcessed) {
      Serial.println("Card removed, you can place a new card");
      cardProcessed = false;
    }
    delay(100);
    return;
  }

  // Try to select the card
  if (!rfid.PICC_ReadCardSerial()) {
    delay(100);
    return;
  }

  // Check if it's the same card (compare UID)
  bool isSameCard = true;
  for (byte i = 0; i < 4 && i < rfid.uid.size; i++) {
    if (lastCardUID[i] != rfid.uid.uidByte[i]) {
      isSameCard = false;
      break;
    }
  }

  // If card has been processed and it's the same card, don't process again
  if (cardProcessed && isSameCard) {
    Serial.println("Please remove current card before placing a new one");
    delay(500);
    rfid.PICC_HaltA();
    rfid.PCD_StopCrypto1();
    return;
  }

  // Display original UID
  Serial.print(F("Card UID:"));
  dump_byte_array(rfid.uid.uidByte, rfid.uid.size);
  Serial.println();
  Serial.print(F("PICC type: "));
  MFRC522::PICC_Type piccType = rfid.PICC_GetType(rfid.uid.sak);
  Serial.println(rfid.PICC_GetTypeName(piccType));

  // Try to write Block 0
  if (writeBlock0(newUid)) {
    Serial.println("UID written successfully!");
  } else {
    Serial.println("Write failed, please check if the card supports UID writing.");
  }

  // Save current card's UID for comparison
  for (byte i = 0; i < 4 && i < rfid.uid.size; i++) {
    lastCardUID[i] = rfid.uid.uidByte[i];
  }
  
  // Mark card as processed
  cardProcessed = true;

  // Stop card communication, but doesn't prevent next read
  rfid.PICC_HaltA();
  rfid.PCD_StopCrypto1(); // Close the encryption
  delay(100);
}

// Custom function: Write Block 0 (includes UID and BCC)
bool writeBlock0(byte* uid) {
  MFRC522::StatusCode status;

  byte block0Data[16];

  // UID (4 bytes)
  memcpy(block0Data, uid, 4);

  // BCC (1 byte)
  block0Data[4] = uid[0] XOR uid[1] XOR uid[2] XOR uid[3]; // Calculate BCC checksum

  // SAK (1 byte)
  block0Data[5] = 0x08;

  // ATQA (2 bytes)
  block0Data[6] = 0x04;
  block0Data[7] = 0x00;

  // Manufacturer (8 bytes)
  byte manufacturer[] = {0x62, 0x63, 0x64, 0x65, 0x66, 0x67, 0x68, 0x69};
  memcpy(&block0Data[8], manufacturer, 8);

  // Select card
  status = rfid.PCD_Authenticate(MFRC522::PICC_CMD_MF_AUTH_KEY_A, 3, &defaultKey, &(rfid.uid));
  if (status != MFRC522::STATUS_OK) {
    Serial.print("Authentication failed: ");
    Serial.println(rfid.GetStatusCodeName(status));
    return false;
  }

  // Write Block 0
  status = rfid.MIFARE_Write(0, block0Data, 16); 
  if (status != MFRC522::STATUS_OK) {
    Serial.print("Write failed: ");
    Serial.println(rfid.GetStatusCodeName(status));
    return false;
  }

  return true;
}

void dump_byte_array(byte *buffer, byte bufferSize) {
    for (byte i = 0; i < bufferSize; i++) {
        Serial.print(buffer[i] < 0x10 ? " 0" : " ");
        Serial.print(buffer[i], HEX);
    }
}
