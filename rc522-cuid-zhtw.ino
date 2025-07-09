#include <SPI.h>
#include <MFRC522.h>

#define XOR ^
// 各平台引腳定義
#ifdef ESP32
  #ifdef CONFIG_IDF_TARGET_ESP32S3
    // ESP32S3
    // 建議只能使用Pins 4,5,13,14,16,17,18,19,21,22,23,25,26,27,32,33
    #define RST_PIN 4
    #define SDA_PIN 5
    #define MISO_PIN 16
    #define SCK_PIN 17
    #define MOSI_PIN 18
    #define PLATFORM_NAME "ESP32S3"
  #else
    // ESP32
    // 建議只能使用Pins 2,4,5,13,14,15,16,17,18,19,21,22,23,25,26,27,32,33
    #define RST_PIN 4
    #define SDA_PIN 5
    #define MISO_PIN 19    // ESP32 VSPI MISO
    #define SCK_PIN 18     // ESP32 VSPI SCK
    #define MOSI_PIN 23    // ESP32 VSPI MOSI
    #define PLATFORM_NAME "ESP32"
  #endif
#else
  // Arduino (Uno/Nano/等) 引腳配置
  /*
   * Arduino 的SPI引腳是固定的，不能更改，僅能更改RST和SDA
   * 資料來源: https://github.com/miguelbalboa/rfid#pin-layout
   * 典型引腳配置：
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
// 通常預設金鑰
MFRC522::MIFARE_Key defaultKey = {{0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF}};

// 要寫入的新卡號 (4 bytes) 和 BCC 校驗位元（自動計算）
byte newUid[] = {0x12, 0x34, 0x56, 0x69}; // 請替換為您的卡號

// 狀態變數
bool cardProcessed = false;
byte lastCardUID[4] = {0, 0, 0, 0};

void setup() {
  Serial.begin(9600);
  #ifdef ESP32
    // ESP32/ESP32S3 初始化
    SPI.begin(SCK_PIN, MISO_PIN, MOSI_PIN, SDA_PIN);
  #else
    // Arduino 初始化
    while (!Serial);     // 等待串口開啟
    SPI.begin();         // 初始化 SPI 匯流排
  #endif
  mfrc522.PCD_Init();
  delay(4);
  Serial.println("警告：此程式會覆寫您的CUID卡的卡號，請謹慎使用！");
  while(mfrc522.PCD_PerformSelfTest() == false) {
    Serial.println("自檢失敗，請檢查線路連接是否正常...");
    delay(1000);
  }
  Serial.println("自檢成功，可以開始寫入卡號，請放上卡片");
}

void loop() {
  // 檢查是否有卡片存在
  bool cardPresent = mfrc522.PICC_IsNewCardPresent();
  
  // 如果沒有卡片，重置處理狀態
  if (!cardPresent) {
    if (cardProcessed) {
      Serial.println("您可以放置新卡片");
      cardProcessed = false;
    }
    delay(100);
    return;
  }

  // 嘗試選擇卡片
  if (!mfrc522.PICC_ReadCardSerial()) {
    delay(100);
    return;
  }

  // 檢查是否為相同卡片
  bool isSameCard = true;
  for (byte i = 0; i < 4 && i < mfrc522.uid.size; i++) {
    if (lastCardUID[i] != mfrc522.uid.uidByte[i]) {
      isSameCard = false;
      break;
    }
  }

  // 如果卡片已處理且為相同卡片，不再重複處理
  if (cardProcessed && isSameCard) {
    Serial.println("請先移除當前卡片");
    delay(500);
    mfrc522.PICC_HaltA();
    mfrc522.PCD_StopCrypto1();
    return;
  }

  // 顯示原始卡號和類型
  Serial.print("原始卡號: ");
  dump_byte_array(mfrc522.uid.uidByte, mfrc522.uid.size);
  Serial.println();
  Serial.print("卡片類型: ");
  MFRC522::PICC_Type piccType = mfrc522.PICC_GetType(mfrc522.uid.sak);
  Serial.println(mfrc522.PICC_GetTypeName(piccType));

  // 嘗試寫入 Block 0
  if (writeBlock0(newUid)) {
    Serial.println("卡號變更成功！");
  } else {
    Serial.println("卡號變更失敗！");
  }

  // 保存當前卡片的卡號以供比較
  for (byte i = 0; i < 4 && i < mfrc522.uid.size; i++) {
    lastCardUID[i] = mfrc522.uid.uidByte[i];
  }
  
  // 標記卡片已處理
  cardProcessed = true;

  // 停止卡片通訊，但不阻止下次讀取
  mfrc522.PICC_HaltA();
  mfrc522.PCD_StopCrypto1(); // 關閉加密
  delay(100);
}

// 寫入 Block 0 的子程式
bool writeBlock0(byte* uid) {
  MFRC522::StatusCode status; // 狀態變數

  byte block0Data[16];

  // 卡號 (4 bytes) 請勿調整成00000000，這可能導致卡片損毀
  memcpy(block0Data, uid, 4);

  // BCC (1 byte) 此處邏輯請勿調整，否則可能導致卡片損毀
  block0Data[4] = uid[0] XOR uid[1] XOR uid[2] XOR uid[3]; // 計算 BCC 校驗位元

  // SAK (1 byte) 請勿調整成18或20或28或88，這可能導致卡片損毀
  block0Data[5] = 0x08;

  // ATQA (2 bytes) 請勿調整成4400，這可能導致卡片損毀
  block0Data[6] = 0x04;
  block0Data[7] = 0x00;

  // 製造商數據 (8 bytes)
  byte manufacturer[] = {0x62, 0x63, 0x64, 0x65, 0x66, 0x67, 0x68, 0x69};
  memcpy(&block0Data[8], manufacturer, 8);

  // 選擇卡片並認證sector 0的密鑰
  status = mfrc522.PCD_Authenticate(MFRC522::PICC_CMD_MF_AUTH_KEY_A, 3, &defaultKey, &(mfrc522.uid));
  if (status != MFRC522::STATUS_OK) {
    Serial.print("認證失敗: ");
    Serial.println(mfrc522.GetStatusCodeName(status));
    Serial.println("建議: 請檢查RC522接線是否正常，以及卡片的密鑰是否為預設值");
    return false;
  }

  // 寫入 Block 0
  status = mfrc522.MIFARE_Write(0, block0Data, 16); 
  if (status != MFRC522::STATUS_OK) {
    Serial.print("寫入失敗: ");
    Serial.println(mfrc522.GetStatusCodeName(status));
    Serial.println("建議: 請檢查卡片是否為CUID卡");
    return false;
  }

  return true;
}

// 輸出16進位的子程式
void dump_byte_array(byte *buffer, byte bufferSize) {
    for (byte i = 0; i < bufferSize; i++) {
        Serial.print(buffer[i] < 0x10 ? " 0" : " ");
        Serial.print(buffer[i], HEX);
    }
} 