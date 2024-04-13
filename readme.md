
```cpp
#define buzzer 3
SoftwareSerial softSerial(/*rx =*4, /*tx =*/5);
#define PIN_DMD_A         6    // D6
#define PIN_DMD_B         7    // D7
#define PIN_DMD_SCLK      8    // D8
#define PIN_DMD_nOE       9    // D9 active low Output Enable, setting this low lights all the LEDs in the selected 
//Define this chip select pin that the Ethernet W5100 IC or other SPI device uses
//if it is in use during a DMD scan request then scanDisplayBySPI() will exit without conflict! (and skip that scan)
#define PIN_OTHER_SPI_nCS 10
#define PIN_DMD_R_DATA    11   // D11_MOSI is SPI Master Out if SPI is used
#define LED_PIN 12
#define PIN_DMD_CLK       13   // D13_SCK  is SPI Clock if SPI is used
#define PIR_SENSOR1_PIN A0 // Pin for PIR motion sensor 1
#define PIR_SENSOR2_PIN A1 // Pin for PIR motion sensor 2
#define RESTART_BUTTON_PIN A2 // Pin for reset button
```
