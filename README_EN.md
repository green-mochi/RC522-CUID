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

## Modify Write Key

Modify the `defaultKey` array in the code:

```cpp
MFRC522::MIFARE_Key defaultKey = {{0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF}};
```

Other possible keys for reference:
```cpp
A0 A1 A2 A3 A4 A5
B0 B1 B2 B3 B4 B5
4D 3A 99 C3 51 DD
1A 98 2C 7E 45 9A
D3 F7 D3 F7 D3 F7
AA BB CC DD EE FF
00 00 00 00 00 00
```

## How to Use

1. Download the [example program](https://github.com/green-mochi/RC522-CUID/blob/main/rc522-cuid-en.ino) according to your language
2. Complete the wiring according to the pin connection instructions
3. Open and upload the program using Arduino IDE
4. Open the Serial Monitor (choose baud rate: 9600)
5. Place a card near the reader
6. If the conditions are met, the program will automatically write the new card number and display the result

## Troubleshooting

* **Card not detected**: Double-check your wiring and ensure the card is a MIFARE-type
* **UID write failed**: Make sure you are using a Gen2 Magic card (CUID) that supports writing to block 0
* **Same card keeps being detected**: The program prevents duplicate writes; remove the card completely before trying again
* **Unstable communication**: Check if jumper wires are loose
* **Board not found**: First confirm that the drivers are installed, then check if the COM port and board options are selected correctly. If the board cannot be found, you need to manually install the board programmer in `Tools > Board > Boards Manager`
* **Board detected but code upload fails**: Confirm whether the board needs a button press during upload, or if the board has two USB ports, make sure you're using the correct programming port

## Legal Disclaimer

⚠️ Modifying the UID of RFID cards may be illegal in some countries or regions. This project is intended for educational and lawful use only. Please comply with your local laws and regulations.

## Acknowledgments

This project is based on the [MFRC522 library](https://github.com/miguelbalboa/rfid) and includes modifications to support writable UID cards.

## License

This project is released under the GPLv3.0 license.
