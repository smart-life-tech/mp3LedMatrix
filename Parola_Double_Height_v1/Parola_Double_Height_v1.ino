
// The upper and lower portions are managed as 2 zones 'stacked' on top of each other
// so that the module numbers are in the sequence shown below:
//
// * Modules (like FC-16) that can fit over each other with no gap
//  n n-1 n-2 ... n/2+1   <- this direction top row
//  n/2 ... 3  2  1  0    <- this direction bottom row
//
// * Modules (like Generic and Parola) that cannot fit over each other with no gap
//  n/2+1 ... n-2 n-1 n   -> this direction top row
//  n/2 ... 3  2  1  0    <- this direction bottom row


#include <MD_Parola.h>
#include <MD_MAX72xx.h>
#include <SPI.h>
#include "Font_Data.h"

#define DEBUG 0

// Define the number of devices we have in the chain and the hardware interface
// NOTE: These pin numbers will probably not work with your hardware and may
// need to be adapted
#define HARDWARE_TYPE MD_MAX72XX::FC16_HW
#define MAX_ZONES 2
#define ZONE_SIZE 4
#define MAX_DEVICES (MAX_ZONES * ZONE_SIZE)
#define SCROLL_SPEED  30

#define ZONE_UPPER  1
#define ZONE_LOWER  0

#define CLK_PIN   13
#define DATA_PIN  11
#define CS_PIN    10

// HARDWARE SPI
MD_Parola P = MD_Parola(HARDWARE_TYPE, CS_PIN, MAX_DEVICES);
// SOFTWARE SPI
//MD_Parola P = MD_Parola(HARDWARE_TYPE, DATA_PIN, CLK_PIN, CS_PIN, MAX_DEVICES);

// Hardware adaptation parameters for scrolling
bool invertUpperZone = false;

#define ARRAY_SIZE(a) (sizeof(a)/sizeof(a[0]))
char *msg[] =
{
  "Create double height displays using 2 custom fonts and 2 zones",
  "Zone 0 for lower half",
  "Zone 1 for upper half",
  "BIG",
  "ABCDEFGHIJKLMNOPQRSTUVWXYZ",
  "abcdefghijklmnopqrstuvwxyz",
  "0123456789",
  "`!@#$%^&*()_+-={};:'\"<>?,./|\\{}",
  "Download Parola at github.com/MajicDesigns",
  "Watch the video on YouTube"
};

void setup(void)
{
  invertUpperZone = (HARDWARE_TYPE == MD_MAX72XX::GENERIC_HW || HARDWARE_TYPE == MD_MAX72XX::PAROLA_HW);

#if DEBUG
  Serial.begin(57600);
  Serial.println("[Double Height demo start]");
#endif

  // initialise the LED display
  P.begin(MAX_ZONES);

  // Set up zones for 2 halves of the display
  // Each zone gets a different font, making up the top
  // and bottom half of each letter
  P.setZone(ZONE_LOWER, 0, ZONE_SIZE - 1);
  P.setFont(ZONE_LOWER, BigFontLower);

  P.setZone(ZONE_UPPER, ZONE_SIZE, MAX_DEVICES-1);
  P.setFont(ZONE_UPPER, BigFontUpper);
  P.setCharSpacing(P.getCharSpacing() * 2); // double height --> double spacing
  if (invertUpperZone)
  {
    P.setZoneEffect(ZONE_UPPER, true, PA_FLIP_UD);
    P.setZoneEffect(ZONE_UPPER, true, PA_FLIP_LR);
  }
}

void loop(void)
{
  static uint8_t cycle = 0;

  // Run the animation and then check if BOTH zones have
  // completed. The animations are not the same length due
  // to upper/lower effects being displayed differently.
  P.displayAnimate();

  if (P.getZoneStatus(ZONE_LOWER) && P.getZoneStatus(ZONE_UPPER))
  {
#if DEBUG
    Serial.println(cycle);
#endif

    switch (cycle)
    {
    default:
      P.setFont(ZONE_LOWER, BigFontLower);
      P.setFont(ZONE_UPPER, BigFontUpper);
      if (invertUpperZone)
      {
        P.displayZoneText(ZONE_LOWER, msg[cycle], PA_LEFT, SCROLL_SPEED, 0, PA_SCROLL_LEFT, PA_SCROLL_LEFT);
        P.displayZoneText(ZONE_UPPER, msg[cycle], PA_LEFT, SCROLL_SPEED, 0, PA_SCROLL_RIGHT, PA_SCROLL_RIGHT);
      }
      else
      {
        P.displayZoneText(ZONE_LOWER, msg[cycle], PA_RIGHT, SCROLL_SPEED, 0, PA_SCROLL_LEFT, PA_SCROLL_LEFT);
        P.displayZoneText(ZONE_UPPER, msg[cycle], PA_LEFT, SCROLL_SPEED, 0, PA_SCROLL_LEFT, PA_SCROLL_LEFT);
      }
      break;

    case 1:
      P.setFont(ZONE_LOWER, NULL);
      P.setFont(ZONE_UPPER, BigFontUpper);
      P.displayZoneText(ZONE_LOWER, msg[1], PA_CENTER, SCROLL_SPEED, 0, PA_SCROLL_LEFT, PA_SCROLL_LEFT);
      P.displayZoneText(ZONE_UPPER, msg[3], PA_CENTER, SCROLL_SPEED, 0, PA_PRINT, PA_NO_EFFECT);
      break;

    case 2:
      P.setFont(ZONE_LOWER, BigFontLower);
      P.setFont(ZONE_UPPER, NULL);
      P.displayZoneText(ZONE_LOWER, msg[3], PA_CENTER, SCROLL_SPEED, 0, PA_PRINT, PA_NO_EFFECT);
      if (invertUpperZone)
        P.displayZoneText(ZONE_UPPER, msg[2], PA_CENTER, SCROLL_SPEED, 0, PA_SCROLL_RIGHT, PA_SCROLL_RIGHT);
      else
        P.displayZoneText(ZONE_UPPER, msg[2], PA_CENTER, SCROLL_SPEED, 0, PA_SCROLL_LEFT, PA_SCROLL_LEFT);
      break;

    case 3:
      P.setFont(ZONE_LOWER, BigFontLower);
      P.setFont(ZONE_UPPER, BigFontUpper);
      P.displayZoneText(ZONE_LOWER, msg[3], PA_CENTER, SCROLL_SPEED, 2000, PA_PRINT, PA_NO_EFFECT); // PA_SCROLL_LEFT, PA_SCROLL_LEFT);
      P.displayZoneText(ZONE_UPPER, msg[3], PA_CENTER, SCROLL_SPEED, 2000, PA_PRINT, PA_NO_EFFECT); //PA_SCROLL_RIGHT, PA_SCROLL_RIGHT);
      break;
    }

    // prepare for next pass
    cycle = (cycle + 1) % ARRAY_SIZE(msg);

    // synchronise the start
    P.displayClear();
    P.synchZoneStart();
  }
}
