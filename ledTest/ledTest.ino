#include <MD_Parola.h>
#include <MD_MAX72xx.h>
#include <SPI.h>

#define MAX_DEVICES 4 // Number of connected 788AS modules per display

#define HARDWARE_TYPE MD_MAX72XX::FC16_HW

#define CLK_PIN   13  // Pin connected to the CLK pin of MAX7219
#define DATA_PIN  11  // Pin connected to the DATA pin of MAX7219
#define CS_PIN    10  // Pin connected to the CS pin of the first display
#define CS_PIN2    9  // Pin connected to the CS pin of the second display

MD_Parola myDisplay = MD_Parola(HARDWARE_TYPE, CS_PIN, MAX_DEVICES);
MD_Parola myDisplay2 = MD_Parola(HARDWARE_TYPE, CS_PIN2, MAX_DEVICES);

void setup() {
  Serial.begin(9600);

  // Initialize the first display
  myDisplay.begin();
  myDisplay.setIntensity(0); // Set the brightness of the first display (0-15)

  // Initialize the second display
  myDisplay2.begin();
  myDisplay2.setIntensity(0); // Set the brightness of the second display (0-15)

  // Display text on each display
  myDisplay.displayText("Hello", PA_CENTER, 1000, 0, PA_SCROLL_LEFT);
  myDisplay2.displayText("World!", PA_CENTER, 1000, 0, PA_SCROLL_LEFT);
}

void loop() {
  // Update display animations for the first display
  myDisplay.displayAnimate();

  // Update display animations for the second display
  myDisplay2.displayAnimate();
}
