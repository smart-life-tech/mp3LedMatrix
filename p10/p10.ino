#include <SPI.h>
#include <EEPROM.h>
#include "Arduino.h"
#include "DFRobotDFPlayerMini.h"
#include <FastLED.h>
#include <Tone.h>
#include <DMD.h>
#include <TimerOne.h>
#include "Arial_black_16.h"
#include "Arial_Black_16_ISO_8859_1.h"
#include "Arial14.h"
#include "SystemFont5x7.h"

#define DISPLAYS_ACROSS 1 //-> Number of P10 panels used, side to side.
// int speed1 = 100;
int speed2 = 20;
int speed3 = 30;
int speed4 = 40;
int speed5 = 50;
int speed6 = 60;
int speed7 = 70;
int speed8 = 80;
int speed9 = 90;
int speed10 = 100;

#define DISPLAYS_DOWN 2
DMD dmd(DISPLAYS_ACROSS, DISPLAYS_DOWN);

char *Text = "";

Tone buzzerr; // buxxer on pin 3
#define buzzer A3
#define LED_PIN A4
#define NUM_LEDS 10
#define LED_TYPE WS2812
#define COLOR_ORDER GRB

CRGB leds[NUM_LEDS];

#if (defined(ARDUINO_AVR_UNO) || defined(ESP8266)) // Using a soft serial port
#include <SoftwareSerial.h>
SoftwareSerial softSerial(/*rx =*/3, /*tx =*/2);
#define FPSerial softSerial
#else
#define FPSerial Serial1
#endif

DFRobotDFPlayerMini myDFPlayer;
void printDetail(uint8_t type, int value);

#define PIR_SENSOR1_PIN A0 // Pin for PIR motion sensor 1
#define PIR_SENSOR2_PIN A1 // Pin for PIR motion sensor 2

#define RESTART_BUTTON_PIN A2 // Pin for reset button

char message[] = "GO GO GO";
char wonMessage[] = "BooM  ";
char lossMessage[] = "Fail   ";
char maxMessage[10] = "";
uint16_t messageLength;

int score = 0;
int defaultMinRange = 0;
int defaultMaxRange = 1000;
int defaultMinOutput = 0;
int defaultMaxOutput = 999;

unsigned long firstSensorTime = 0;  // Time when first sensor detects movement
unsigned long secondSensorTime = 0; // Time when second sensor detects movement

unsigned long previousMillis = 0; // Variable to store the previous time
unsigned long interval = 1500;    // Interval in milliseconds
int currentText = 0;
int maxScore = 0;
void ScanDMD()
{
    dmd.scanDisplayBySPI();
}
void setup()
{
    Serial.begin(115200);
    Serial.println("code started");
    mp3setup();
    pinMode(PIR_SENSOR1_PIN, INPUT);
    pinMode(PIR_SENSOR2_PIN, INPUT);

    pinMode(RESTART_BUTTON_PIN, INPUT_PULLUP);
    pinMode(buzzer, OUTPUT);
    buzzerr.begin(buzzer);
    buzzerFunc(HIGH);
    EEPROM.begin();
    Timer1.initialize(1000);
    Timer1.attachInterrupt(ScanDMD);
    dmd.clearScreen(true);
    dmd.selectFont(Arial_Black_16);
    maxScore = read();
    // buf[19] = '\0';
    sprintf(maxMessage, "%d", maxScore);
    dmd.drawString(3, 18, maxMessage, 3, GRAPHICS_NORMAL);
    Serial.println(read());

    FastLED.addLeds<LED_TYPE, LED_PIN, COLOR_ORDER>(leds, NUM_LEDS);
    if (read() >= 999)
    {
        Serial.println("score is greater than 999 formatting eeprom");
        write(0);
    }

    Serial.println("code started");
    Serial.println(maxMessage);
    colorWipe(CRGB::Black, 50, NUM_LEDS); // Off

    delay(4000);
    Serial.println("code started finished final set up");
}
bool anim = true;
void loop()
{
    bool lossPlaying = true;
    bool wonPlaying = true;
    bool playing = true;
    // myDisplay.displayReset();
    // dmd.drawString(3, 0, maxMessage, 3, GRAPHICS_NORMAL);
    if (anim)
    {
        dmd.drawMarquee(message, strlen(message), (3 * DISPLAYS_ACROSS) - 1, 0);

        boolean ret = false;
        while (!ret)
        {
            dmd.drawString(3, 18, maxMessage, 3, GRAPHICS_NORMAL);
            ret = dmd.stepMarquee(-1, 0);
            delay(100);
            if (digitalRead(PIR_SENSOR1_PIN) == LOW)
            {
                dmd.drawString(3, 18, "           ", 10, GRAPHICS_NORMAL);
                dmd.drawString(3, 18, "           ", 10, GRAPHICS_NORMAL);
                anim = false;
                break;
            }
        }
    }

    // Check if the first sensor detects movement
    if (digitalRead(PIR_SENSOR1_PIN) == LOW && firstSensorTime == 0)
    {
        firstSensorTime = millis(); // Record the time when first sensor detects movement
        Serial.println("First sensor detected movement");
        if (playing)
        {
            myDFPlayer.loop(4);
            playing = false;
            dmd.clearScreen(true);
        }
        // myDFPlayer.stop(); // stop playing
    }

    // Check if the second sensor detects movement
    if (digitalRead(PIR_SENSOR2_PIN) == LOW && firstSensorTime > 0 && secondSensorTime == 0)
    {
        secondSensorTime = millis();                                       // Record the time when second sensor detects movement
        unsigned long timeDifference = secondSensorTime - firstSensorTime; // Calculate the time difference

        if (timeDifference > 500)
        {
            timeDifference = timeDifference * 1.5;
        }
        else if (timeDifference > 400)
        {
            timeDifference = timeDifference * 2;
        }
        else if (timeDifference > 300)
        {
            timeDifference = timeDifference * 2.5;
        }
        else if (timeDifference > 200)
        {
            timeDifference = timeDifference * 4;
        }
        else
        {
            timeDifference = timeDifference * 8;
        }

        // Map the timeDifference to a reversed range of 0-999
        int mappedScore;

        if (timeDifference > 1000)
        {
            mappedScore = 1;
        }
        else
        {
            Serial.print("time difference : ");
            Serial.println(timeDifference);
            mappedScore = map(timeDifference, 80, 1000, 999, 1);
            Serial.print("mapped score : ");
            Serial.println(mappedScore);
            // checkscore(mappedScore);
        }

        if (mappedScore >= 999)
        {
            mappedScore = 999;
        }

        while (score != mappedScore)
        {
            // Check if reset button is pressed
            if (digitalRead(RESTART_BUTTON_PIN) == LOW)
            {
                rebootArduino();
            }
            score++;
            buzzerFunc(HIGH);
            int bar = map(score, 0, 999, 0, NUM_LEDS);
            colorWipe(CRGB::Red, 60, bar);
            sprintf(message, "%d   ", score);
            // dmd.clearScreen(true);
            dmd.drawString(3, 18, maxMessage, 9, GRAPHICS_NORMAL);
            dmd.drawString(3, 0, message, 10, GRAPHICS_NORMAL);

            if ((score) <= 100)
            {
                delay(speed2);
                Serial.println("speed 100");
            }
            else if ((score) <= 200)
            {
                delay(speed3);
                Serial.println("speed 200");
            }
            else if ((score) <= 300)
            {
                delay(speed4);
                Serial.println("speed 300");
            }

            else if ((score) <= 400)
            {
                delay(speed5);
                Serial.println("speed 400");
            }
            else if ((score) <= 500)
            {
                delay(speed6);
                Serial.println("speed 500");
            }
            else if ((score) <= 600)
            {
                delay(speed7);
                Serial.println("speed 600");
            }

            else if ((score) <= 700)
            {
                delay(speed8);
                Serial.println("speed 800");
            }
            else if ((score) <= 800)
            {
                delay(speed9);
                Serial.println("speed 900");
            }
            else if ((score) <= 900)
            {
                delay(speed10);
                Serial.println("speed 1000");
            }

            // checkscore(score);
            /// buzzerFunc(LOW);
            if (score == mappedScore - 1 || score == mappedScore)
            {
                sprintf(message, "%d   ", mappedScore);
                while (true)
                {
                    // Check if reset button is pressed
                    if (digitalRead(RESTART_BUTTON_PIN) == LOW)
                    {
                        rebootArduino();
                    }

                    unsigned long currentMillis = millis(); // Get the current time

                    if (currentMillis - previousMillis >= interval)
                    {
                        previousMillis = currentMillis; // Update the previous time
                        dmd.drawString(3, 0, "           ", 10, GRAPHICS_NORMAL);
                        switch (currentText)
                        {
                        case 0:
                            dmd.drawString(3, 0, "                   ", 20, GRAPHICS_NORMAL);
                            dmd.drawString(3, 0, message, 3, GRAPHICS_NORMAL);
                            checkscore(mappedScore);

                            currentText = 1;
                            break;

                        case 1:
                            if (score < 300)
                            {
                                if (lossPlaying)
                                {
                                    // myDFPlayer.play(0);
                                    myDFPlayer.play(3);
                                    lossPlaying = false;
                                }
                                // dmd.clearScreen(true);
                                dmd.drawString(3, 0, "           ", 10, GRAPHICS_NORMAL);
                                dmd.drawString(0, 0, lossMessage, 50, GRAPHICS_NORMAL);
                            }
                            else
                            {
                                if (wonPlaying)
                                {
                                    myDFPlayer.play(2);
                                    wonPlaying = false;
                                }
                                // dmd.clearScreen(true);
                                dmd.drawString(3, 0, "                ", 70, GRAPHICS_NORMAL);
                                dmd.drawString(0, 0, wonMessage, 50, GRAPHICS_NORMAL);
                            }
                            currentText = 0;
                            break;
                        }
                    }
                }
            }
        }
    }
}
void checkscore(int mappedScore)
{
    if (mappedScore > read())
    {
        write(mappedScore);
        maxScore = mappedScore;
        sprintf(maxMessage, "%d    ", maxScore);
        dmd.drawString(3, 18, "           ", 10, GRAPHICS_NORMAL);
        dmd.drawString(3, 18, maxMessage, 3, GRAPHICS_NORMAL);
    }
    else
    {

        maxScore = read();
        sprintf(maxMessage, "%d    ", maxScore);
        dmd.drawString(3, 18, "           ", 10, GRAPHICS_NORMAL);
        dmd.drawString(3, 18, maxMessage, 3, GRAPHICS_NORMAL);
    }
}
void mp3setup()
{
#if (defined ESP32)
    FPSerial.begin(9600, SERIAL_8N1, /*rx =*/D3, /*tx =*/D2);
#else
    FPSerial.begin(9600);
#endif

    // Serial.begin(115200);

    Serial.println();
    Serial.println(F("DFRobot DFPlayer Mini Demo"));
    Serial.println(F("Initializing DFPlayer ... (May take 3~5 seconds)"));

    if (!myDFPlayer.begin(FPSerial, /*isACK = */ true, /*doReset = */ true))
    { // Use serial to communicate with mp3.
        Serial.println(F("Unable to begin:"));
        Serial.println(F("1.Please recheck the connection!"));
        Serial.println(F("2.Please insert the SD card!"));
        while (true)
            ;
    }
    Serial.println(F("DFPlayer Mini online."));

    myDFPlayer.setTimeOut(500); // Set serial communictaion time out 500ms

    //----Set volume----
    myDFPlayer.volume(10);   // Set volume value (0~30).
    myDFPlayer.volumeUp();   // Volume Up
    myDFPlayer.volumeDown(); // Volume Down

    //----Set different EQ----
    myDFPlayer.EQ(DFPLAYER_EQ_NORMAL);
    //  myDFPlayer.EQ(DFPLAYER_EQ_POP);
    //  myDFPlayer.EQ(DFPLAYER_EQ_ROCK);
    //  myDFPlayer.EQ(DFPLAYER_EQ_JAZZ);
    //  myDFPlayer.EQ(DFPLAYER_EQ_CLASSIC);
    //  myDFPlayer.EQ(DFPLAYER_EQ_BASS);

    //----Set device we use SD as default----
    //  myDFPlayer.outputDevice(DFPLAYER_DEVICE_U_DISK);
    myDFPlayer.outputDevice(DFPLAYER_DEVICE_SD);
    //  myDFPlayer.outputDevice(DFPLAYER_DEVICE_AUX);
    //  myDFPlayer.outputDevice(DFPLAYER_DEVICE_SLEEP);
    //  myDFPlayer.outputDevice(DFPLAYER_DEVICE_FLASH);

    //----Mp3 control----
    //  myDFPlayer.sleep();     //sleep
    //  myDFPlayer.reset();     //Reset the module
    //  myDFPlayer.enableDAC();  //Enable On-chip DAC
    //  myDFPlayer.disableDAC();  //Disable On-chip DAC
    //  myDFPlayer.outputSetting(true, 15); //output setting, enable the output and set the gain to 15
    myDFPlayer.play(1); // Play the first mp3
                        // delay(5000);
                        // myDFPlayer.stop(); // Play the first mp3
}
void rebootArduino()
{
    asm volatile("jmp 0"); // Perform a software reset by jumping to address 0
    Serial.println("reboot button pressed");
}

void write(int number)
{
    if (number == 0 || number == 999)
    {
        EEPROM.write(0, 0);
        EEPROM.write(1, 0);
        EEPROM.write(2, 0);
    }
    else
    {
        EEPROM.write(0, number / 100);
        EEPROM.write(1, (number / 10) % 10);
        EEPROM.write(2, number % 10);
    }
}
int read()
{
    int h = EEPROM.read(0);
    int t = EEPROM.read(1);
    int u = EEPROM.read(2);
    return h * 100 + t * 10 + u;
}
void colorWipe(CRGB color, int wait, int ledss)
{
    for (int i = 0; i < ledss; i++)
    {
        leds[i] = color;
        FastLED.show();
    }
}
void buzzerFunc(bool status)
{
    digitalWrite(buzzer, status);
    // Play a sound when counting the score
    buzzerr.play(1000, 5);
}