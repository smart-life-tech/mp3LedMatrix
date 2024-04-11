#include <SPI.h>
#include <DMD.h>
#include <TimerOne.h>
#include "SystemFont5x7.h"
#include "Arial_black_16.h"
#include "Arial14.h"
#define DISPLAYS_ACROSS 1
#define DISPLAYS_DOWN 1
String str;
char b[8];
DMD dmd(DISPLAYS_ACROSS, DISPLAYS_DOWN);

void ScanDMD()
{
    dmd.scanDisplayBySPI();
}

void setup()
{
    Serial.begin(9600);
    Timer1.initialize(5000);
    Timer1.attachInterrupt(ScanDMD);
    dmd.clearScreen(true);
}

void loop()
{
    int slen = 0;
    dmd.clearScreen(true);
    /*---------------------------------------  Display using SystemFont  -------------------------------------*/
    dmd.selectFont(SystemFont5x7);
    str = "HELLO";
    slen = str.length() + 1;
    str.toCharArray(b, slen);
    dmd.drawString(1, 0, b, slen, GRAPHICS_NORMAL);
    str = "WORLD";
    slen = str.length() + 1;
    str.toCharArray(b, slen);
    dmd.drawString(1, 8, b, slen, GRAPHICS_NORMAL);
    delay(4000);
    dmd.clearScreen(true);
    delay(2000);

    /*---------------------------------  Display using Arial_black_16  -------------------------------------*/
    dmd.selectFont(Arial_Black_16);
    str = "BEP";
    slen = str.length() + 1;
    str.toCharArray(b, slen);
    dmd.drawString(2, 1, b, slen, GRAPHICS_NORMAL);
    delay(4000);
    dmd.clearScreen(true);
    delay(2000);

    /*----------------------------------  Display using  Arial14  -------------------------------------------- */
    dmd.selectFont(Arial_14);
    str = "BEP!";
    slen = str.length() + 1;
    str.toCharArray(b, slen);
    dmd.drawString(4, 1, b, slen, GRAPHICS_NORMAL);
    delay(4000);
    dmd.clearScreen(true);
    delay(2000);
}