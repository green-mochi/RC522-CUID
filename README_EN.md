Use an **Arduino UNO** or **ESP32S3** with the **MFRC522 RFID module** to modify the UID of a **CUID card (Gen2 Magic Card)**.

This project provides example codes for both controllers. Choose the one that matches your development board.

## Hardware Requirements

* Arduino UNO or ESP32S3 development board
* MFRC522 RFID module
* CUID card (Gen2 Magic Card)
* Jumper wires

## Pin Connections

### Arduino UNO (fixed pin configuration)

| UNO Pin | MFRC522 Pin |
| ------- | ----------- |
| 9       | RST         |
| 10      | SDA (SS)    |
| 11      | MOSI        |
| 12      | MISO        |
| 13      | SCK         |
| 3.3V    | 3.3V        |
| GND     | GND         |

### ESP32S3 (customizable pin configuration)

| ESP32S3 Pin | MFRC522 Pin |
| ----------- | ----------- |
| 4           | RST         |
| 5           | SDA (SS)    |
| 16          | MISO        |
| 17          | SCK         |
| 18          | MOSI        |
| 3.3V        | 3.3V        |
| GND         | GND         |

For ESP32S3, please adjust the `SPI` and `MFRC522` pin definitions in the code according to your actual wiring.

## Features

* Read the original UID of a card
* Verify if the card is a modifiable CUID (Gen2 Magic card)
* Write a custom UID to block 0
* Prevent duplicate writing to the same card
* Display write status via serial monitor

## Set Custom UID

Modify the `newUid` array in the code:

```cpp
byte newUid[] = {0xDE, 0xAD, 0xBE, 0xEF};  // Your custom UID
```

## How to Use

1. Choose the correct example sketch for your board:

   * [Arduino UNO Example](https://github.com/green-mochi/RC522-CUID/blob/main/arduino/arduino-rc522-cuid.ino)
   * [ESP32S3 Example](https://github.com/green-mochi/RC522-CUID/blob/main/esp32s3/esp32s3-rc522-cuid.ino)
2. Wire the hardware according to the pin configuration section
3. Open the example in Arduino IDE and upload it to your board
4. Open the Serial Monitor (baud rate: 9600 or 115200 depending on the sketch)
5. Place a card near the reader
6. If the card is compatible, the program will attempt to write the new UID and display the result

## Troubleshooting

* **Card not detected**: Double-check your wiring and ensure the card is a MIFARE-type
* **UID write failed**: Make sure you are using a Gen2 Magic card (CUID) that supports writing to block 0
* **Same card keeps being detected**: The program prevents duplicate writes; remove the card completely before trying again
* **Unstable communication**: Check if jumper wires are loose
* **Board not recognized**: Ensure drivers are installed, and that correct COM port and board type are selected in Arduino IDE. Install the board package via `Tools > Board > Board Manager` if needed
* **Code upload fails**: Some boards require pressing a boot button during upload or using the correct USB port (some have multiple USB interfaces)

## Legal Disclaimer

⚠️ Modifying the UID of RFID cards may be illegal in some countries or regions. This project is intended for educational and lawful use only. Please comply with your local laws and regulations.

## Acknowledgments

This project is based on the [MFRC522 library](https://github.com/miguelbalboa/rfid) and includes modifications to support writable UID cards.

## License

This project is released under the GPLv3.0 license.
