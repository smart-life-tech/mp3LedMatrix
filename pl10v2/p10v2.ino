#include <SPI.h>
#include <DMD.h>
#include <TimerOne.h>
#include "Arial_black_16.h"
#include "Arial_Black_16_ISO_8859_1.h"
#include "Arial14.h"
#include "SystemFont5x7.h"

#define DISPLAYS_ACROSS 1 //-> Number of P10 panels used, side to side.

#define DISPLAYS_DOWN 2
DMD dmd(DISPLAYS_ACROSS, DISPLAYS_DOWN);

char *Text = "";

void ScanDMD()
{
    dmd.scanDisplayBySPI();
}

void setup(void)
{
    Timer1.initialize(1000);
    Timer1.attachInterrupt(ScanDMD);
    dmd.clearScreen(true);
    Serial.begin(115200);
}

void loop(void)
{
    dmd.selectFont(SystemFont5x7);
    //DMD.selectFont(Arial_Black_16);

    dmd.drawString(3, 0, "989", 3, GRAPHICS_NORMAL);
    dmd.drawString(3, 18, "752", 3, GRAPHICS_NORMAL);
    delay(1000);
}