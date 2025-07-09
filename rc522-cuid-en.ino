#include <SPI.h>
#include <MFRC522.h>

#define XOR ^
// Pin definitions for different platforms
#ifdef ESP32
  #ifdef CONFIG_IDF_TARGET_ESP32S3
    // ESP32S3
    // Only pins 4,5,13,14,16,17,18,19,21,22,23,25,26,27,32,33 are recommended
    #define RST_PIN 4
    #define SDA_PIN 5
    #define MISO_PIN 16
    #define SCK_PIN 17
    #define MOSI_PIN 18
    #define PLATFORM_NAME "ESP32S3"
  #else
    // ESP32
    // Only pins 2,4,5,13,14,15,16,17,18,19,21,22,23,25,26,27,32,33 are recommended
    #define RST_PIN 4
    #define SDA_PIN 5
    #define MISO_PIN 19    // ESP32 VSPI MISO
    #define SCK_PIN 18     // ESP32 VSPI SCK
    #define MOSI_PIN 23    // ESP32 VSPI MOSI
    #define PLATFORM_NAME "ESP32"
  #endif
#else
  // Arduino (Uno/Nano/etc.) pin configuration
  /*
   * Arduino SPI pins are fixed and cannot be changed, only RST and SDA can be modified
   * Reference: https://github.com/miguelbalboa/rfid#pin-layout
   * Typical pin configuration:
   * -----------------------------------------------------------------------------------------
   *             MFRC522      Arduino       Arduino   Arduino    Arduino          Arduino
   *             Reader/PCD   Uno/101       Mega      Nano v3    Leonardo/Micro   Pro Micro
   * Signal      Pin          Pin           Pin       Pin        Pin              Pin
   * -----------------------------------------------------------------------------------------
   * RST/Reset   RST          9             5         D9         RESET/ICSP-5     RST
   * SPI SDA(SS) SDA(SS)      10            53        D10        10               10
   * SPI MOSI    MOSI         11 / ICSP-4   51        D11        ICSP-4           16
   * SPI MISO    MISO         12 / ICSP-1   50        D12        ICSP-1           14
   * SPI SCK     SCK          13 / ICSP-3   52        D13        ICSP-3           15
   */
  #define RST_PIN 9
  #define SDA_PIN 10
  #define PLATFORM_NAME "Arduino"
#endif

MFRC522 mfrc522(SDA_PIN, RST_PIN);
// Default key (usually)
MFRC522::MIFARE_Key defaultKey = {{0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF}};

// New card ID to write (4 bytes) and BCC checksum (automatically calculated)
byte newUid[] = {0x12, 0x34, 0x56, 0x69}; // Replace with your desired card ID

// Status variables
bool cardProcessed = false;
byte lastCardUID[4] = {0, 0, 0, 0};

void setup() {
  Serial.begin(9600);
  #ifdef ESP32
    // ESP32/ESP32S3 initialization
    SPI.begin(SCK_PIN, MISO_PIN, MOSI_PIN, SDA_PIN);
  #else
    // Arduino initialization
    while (!Serial);     // Wait for serial port to open
    SPI.begin();         // Initialize SPI bus
  #endif
  mfrc522.PCD_Init();
  delay(4);
  Serial.println("WARNING: This program will overwrite the ID of your magic gen2 card, use with caution!");
  while(mfrc522.PCD_PerformSelfTest() == false) {
    Serial.println("Self-test failed, please check wiring connections...");
    delay(1000);
  }
  Serial.println("Self-test successful, ready to write card number, please place a card");
}

void loop() {
  // Check if a card is present
  bool cardPresent = mfrc522.PICC_IsNewCardPresent();
  
  // If no card is present, reset processing status
  if (!cardPresent) {
    if (cardProcessed) {
      Serial.println("You can place a new card");
      cardProcessed = false;
    }
    delay(100);
    return;
  }

  // Try to select the card
  if (!mfrc522.PICC_ReadCardSerial()) {
    delay(100);
    return;
  }

  // Check if it's the same card
  bool isSameCard = true;
  for (byte i = 0; i < 4 && i < mfrc522.uid.size; i++) {
    if (lastCardUID[i] != mfrc522.uid.uidByte[i]) {
      isSameCard = false;
      break;
    }
  }

  // If card is already processed and it's the same card, don't process again
  if (cardProcessed && isSameCard) {
    Serial.println("Please remove the current card first");
    delay(500);
    mfrc522.PICC_HaltA();
    mfrc522.PCD_StopCrypto1();
    return;
  }

  // Display original card ID
  Serial.print("Original card number: ");
  dump_byte_array(mfrc522.uid.uidByte, mfrc522.uid.size);
  Serial.println();
  Serial.print("Card type: ");
  MFRC522::PICC_Type piccType = mfrc522.PICC_GetType(mfrc522.uid.sak);
  Serial.println(mfrc522.PICC_GetTypeName(piccType));

  // Try to write Block 0
  if (writeBlock0(newUid)) {
    Serial.println("Card number change successful!");
  } else {
    Serial.println("Card number change failed!");
  }

  // Save current card's ID for comparison
  for (byte i = 0; i < 4 && i < mfrc522.uid.size; i++) {
    lastCardUID[i] = mfrc522.uid.uidByte[i];
  }
  
  // Mark card as processed
  cardProcessed = true;

  // Stop card communication, but don't prevent next read
  mfrc522.PICC_HaltA();
  mfrc522.PCD_StopCrypto1(); // Close encryption
  delay(100);
}

// Subroutine to write Block 0
bool writeBlock0(byte* uid) {
  MFRC522::StatusCode status; // Status variable

  byte block0Data[16];

  // card ID (4 bytes) Do not change to 00000000, this may damage the card
  memcpy(block0Data, uid, 4);

  // BCC (1 byte) Do not modify this logic, it may damage the card
  block0Data[4] = uid[0] XOR uid[1] XOR uid[2] XOR uid[3]; // Calculate BCC checksum

  // SAK (1 byte) Do not change to 18, 20, 28, or 88, this may damage the card
  block0Data[5] = 0x08;

  // ATQA (2 bytes) Do not change to 4400, this may damage the card
  block0Data[6] = 0x04;
  block0Data[7] = 0x00;

  // Manufacturer data (8 bytes)
  byte manufacturer[] = {0x62, 0x63, 0x64, 0x65, 0x66, 0x67, 0x68, 0x69};
  memcpy(&block0Data[8], manufacturer, 8);

  // Select card and authenticate sector 0 key
  status = mfrc522.PCD_Authenticate(MFRC522::PICC_CMD_MF_AUTH_KEY_A, 3, &defaultKey, &(mfrc522.uid));
  if (status != MFRC522::STATUS_OK) {
    Serial.print("Authentication failed: ");
    Serial.println(mfrc522.GetStatusCodeName(status));
    Serial.println("Suggestion: Please check if RC522 wiring is correct and if the card key A is the default value (0xFF)");
    return false;
  }

  // Write Block 0
  status = mfrc522.MIFARE_Write(0, block0Data, 16); 
  if (status != MFRC522::STATUS_OK) {
    Serial.print("Write failed: ");
    Serial.println(mfrc522.GetStatusCodeName(status));
    Serial.println("Suggestion: Please check if the card is a magic gen2 card");
    return false;
  }

  return true;
}

// Subroutine to output hexadecimal values
void dump_byte_array(byte *buffer, byte bufferSize) {
    for (byte i = 0; i < bufferSize; i++) {
        Serial.print(buffer[i] < 0x10 ? " 0" : " ");
        Serial.print(buffer[i], HEX);
    }
} 