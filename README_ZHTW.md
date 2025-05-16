使用 **Arduino UNO** 或 **ESP32S3** 搭配 **MFRC522 RFID 模組** 修改 **CUID 卡（Gen2 Magic Card）** 的卡號 UID。

本專案提供對兩種主控板的範例程式，可依需求選擇使用。

## 硬體需求

* Arduino UNO 或 ESP32S3 開發板
* MFRC522 RFID 模組
* CUID 卡（Gen2 Magic Card）
* 杜邦線數條

## 腳位接法

### Arduino UNO 對應腳位（不可自定義）

| UNO 腳位 | MFRC522 腳位 |
| ------ | ---------- |
| 9      | RST        |
| 10     | SDA (SS)   |
| 11     | MOSI       |
| 12     | MISO       |
| 13     | SCK        |
| 3.3V   | 3.3V       |
| GND    | GND        |

### ESP32S3 對應腳位（可自定義）

| ESP32S3 腳位 | MFRC522 腳位 |
| ---------- | ---------- |
| 4          | RST        |
| 5          | SDA (SS)   |
| 16         | MISO       |
| 17         | SCK        |
| 18         | MOSI       |
| 3.3V       | 3.3V       |
| GND        | GND        |

ESP32S3請視實際接線修改程式中的 `SPI` 與 `MFRC522` 腳位定義。

## 功能簡介

* 讀取卡片原始 UID
* 檢查是否為支援修改的 CUID（Gen2 Magic）卡
* 寫入自訂的卡號到區塊 0
* 防止重複寫入同一張卡
* 透過序列埠監控器提供寫卡狀態

## 修改卡號

請修改程式碼中的 `newUid` 陣列：

```cpp
byte newUid[] = {0xDE, 0xAD, 0xBE, 0xEF};  // 自訂 UID
```

## 使用說明

1. 選擇對應的主控板範例（例如：[arduino是這個](https://github.com/green-mochi/RC522-CUID/blob/main/arduino/arduino-rc522-cuid.ino) 或 [esp32s3是這個](https://github.com/green-mochi/RC522-CUID/blob/main/esp32s3/esp32s3-rc522-cuid.ino)）
2. 依照腳位接線說明完成接線
3. 使用 Arduino IDE 開啟並上傳程式
4. 開啟序列埠監控器（選擇速度：9600 或 115200，依範例程式而定）
5. 將卡片靠近讀卡器
6. 若符合條件，程式會自動寫入新 UID 並提示結果

## 疑難排解

* **無法偵測卡片**：請確認接線正確、卡片為 MIFARE 類型
* **UID 寫入失敗**：請確認卡片為 CUID卡（支援直接寫入block 0）
* **重複偵測同一張卡**：程式預設避免重複寫入，請將卡片移開後再靠近
* **通訊不穩定**：請確認杜邦線是否鬆脫
* **找不到板子**：先確認驅動程式是否有安裝，再確認COM號和開發板的選項是否選擇正確。如果找不到板子，要自己在`工具` > `開發板` > `開發板管理員`把開發板燒錄器裝上
* **抓到板子但無法上傳程式碼**：確認板子在上傳時是否需要按按鈕，或是板子上有兩個USB孔，要插對燒錄孔
  
## 安全與法律提醒

⚠️ 修改 UID 在某些國家/地區可能違反法律，請僅於合法與教育用途使用。

## 感謝

本專案基於 [MFRC522 函式庫](https://github.com/miguelbalboa/rfid) 的範例進行改寫，並針對可改寫 UID 的卡片進行實作與驗證。

## license

Unlicense license
