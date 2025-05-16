# RC522-changeidesp32

This project allows you to modify the UID of RFID cards using an ESP32S3 microcontroller and an MFRC522 RFID reader module. It's specifically designed to work with Gen2 (Chinese Magic) RFID cards that have writable sector 0.

## Hardware Requirements

- ESP32S3 development board
- MFRC522 RFID reader module
- RFID cards (Gen2 / Magic Cards with writable UID aka "CUID")
- Jumper wires for connections

## Pin Connections

| ESP32S3 Pin | MFRC522 Pin |
|-------------|-------------|
| 4           | RST         |
| 5           | SDA (SS)    |
| 16          | MISO        |
| 17          | SCK         |
| 18          | MOSI        |
| 3.3V        | 3.3V        |
| GND         | GND         |

## Features

- Reads the original UID of a card
- Writes a new predefined UID to the card
- Verifies the authenticity of the card before writing
- Prevents accidental rewriting to the same card (card must be removed before another card can be processed)
- Provides clear user feedback via Serial Monitor

## Configuration

To customize the new UID you want to write to cards, modify the `newUid` array in the code:

```cpp
// Replace with your desired UID
byte newUid[] = {0x12, 0x34, 0x56, 0x69};
```

## Installation

1. Install the Arduino IDE
2. Install the ESP32 board package:
   - In Arduino IDE, go to `File > Preferences`
   - Add `https://raw.githubusercontent.com/espressif/arduino-esp32/gh-pages/package_esp32_index.json` to the "Additional Boards Manager URLs" field
   - Go to `Tools > Board > Boards Manager`
   - Search for "esp32" and install the ESP32 board package
3. Install the MFRC522 library:
   - Go to `Sketch > Include Library > Manage Libraries`
   - Search for "MFRC522" and install the library by Miguel Balboa
4. Download the [app.ino](https://github.com/green-mochi/RC522_change_uid/blob/main/app.ino) from this project and upload it to your ESP32S3 board
5. Open the Serial Monitor (baud rate: 9600) to see the program's output

## Usage

1. Connect the hardware as described in the Pin Connections section
2. Upload the code to your ESP32S3 board
3. Open the Serial Monitor at 9600 baud
4. Place a compatible RFID card on the reader
5. The program will read the original UID and attempt to write the new UID
6. If successful, the program will display "UID written successfully!"
7. Remove the card completely from the reader before placing another card

## How It Works

The program performs the following operations:

1. Initializes the MFRC522 RFID reader and prepares the default keys for authentication
2. Continuously checks for the presence of RFID cards
3. When a card is detected, it reads the current UID
4. It verifies if the card is new or the same as previously processed
5. For new cards, it attempts to write the specified UID to Block 0
6. It tracks card status to ensure proper processing flow
7. Provides user feedback through the Serial Monitor

## Troubleshooting

- **Card Not Detected**: Ensure proper wiring connections and check that the card is compatible
- **Write Failed**: Not all cards support UID modification. Only Gen2/Magic Cards typically allow this
- **Same Card Processed Multiple Times**: This program is designed to prevent this. Remove the card completely from the reader field before placing it again
- **Communication Error**: If you encounter repeated communication errors, try resetting the ESP32 board

## Warning

Modifying card UIDs might be illegal in some jurisdictions or violate terms of service for systems using these cards. This code is provided for educational purposes only. Use responsibly and ethically.

## Credits

Based on the [MFRC522](https://github.com/miguelbalboa/rfid) library examples and modified for ESP32S3 with specific card processing logic.

