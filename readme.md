
# The files on the SD card for the DFPlayer Mini should follow this naming structure:

- /mp3/001.mp3
- /mp3/002.mp3
- /mp3/003.mp3
...
- /mp3/100.mp3
- /mp3/101.mp3
...
- /mp3/298.mp3


# Key points for SD card file naming:

Create an "mp3" folder in the root directory
Name files with 3 digits (001, 002, etc.)
Use .mp3 file extension
Files must be in MP3 format
Numbers must match the track numbers used in the code (100-298)
Format SD card as FAT32
The DFPlayer will read these files sequentially based on the numerical order. This naming convention ensures reliable playback and matches the track numbers referenced in the code

```cpp
#define buzzer A3
SoftwareSerial softSerial(/*rx =*3, /*tx =*/2);
#define PIN_DMD_A         6    // D6
#define PIN_DMD_B         7    // D7
#define PIN_DMD_SCLK      8    // D8
#define PIN_DMD_nOE       9    // D9 active low Output Enable, setting this low lights all the LEDs in the selected 
//Define this chip select pin that the Ethernet W5100 IC or other SPI device uses
//if it is in use during a DMD scan request then scanDisplayBySPI() will exit without conflict! (and skip that scan)
#define PIN_OTHER_SPI_nCS 10
#define PIN_DMD_R_DATA    11   // D11_MOSI is SPI Master Out if SPI is used
#define LED_PIN A4
#define PIN_DMD_CLK       13   // D13_SCK  is SPI Clock if SPI is used
#define PIR_SENSOR1_PIN A0 // Pin for PIR motion sensor 1
#define PIR_SENSOR2_PIN A1 // Pin for PIR motion sensor 2
#define RESTART_BUTTON_PIN A2 // Pin for reset button
```
