#include <MD_Parola.h>
#include <MD_MAX72xx.h>
#include <SPI.h>
#include <EEPROM.h>
#include "Arduino.h"
#include "DFRobotDFPlayerMini.h"
#if (defined(ARDUINO_AVR_UNO) || defined(ESP8266)) // Using a soft serial port
#include <SoftwareSerial.h>
SoftwareSerial softSerial(/*rx =*/5, /*tx =*/6);
#define FPSerial softSerial
#else
#define FPSerial Serial1
#endif

DFRobotDFPlayerMini myDFPlayer;
void printDetail(uint8_t type, int value);

#define CLK_PIN 13  // CLK or SCK
#define DATA_PIN 11 // DIN or MOSI
#define CS_PIN 10   // CS or SS
#define MAX_DEVICES 4
#define CS_PIN2 9         // Pin connected to the CS pin of the second display
#define PIR_SENSOR1_PIN 3 // Pin for PIR motion sensor 1
#define PIR_SENSOR2_PIN 4 // Pin for PIR motion sensor 2

#define RESTART_BUTTON_PIN 2 // Pin for reset button
#define HARDWARE_TYPE MD_MAX72XX::FC16_HW
MD_Parola myDisplay = MD_Parola(MD_MAX72XX::FC16_HW, CS_PIN, MAX_DEVICES);
// Adjust the MAX_DEVICES value to match the number of connected 788AS modules
MD_Parola myDisplay2 = MD_Parola(HARDWARE_TYPE, CS_PIN2, MAX_DEVICES);
const uint8_t charWidth = 6;                  // Width of each character in pixels
const uint8_t displayWidth = MAX_DEVICES * 8; // Total width of the display in pixels

char message[] = "GO GO GO";
char wonMessage[] = "BooM";
char lossMessage[] = "Fail";
char maxMessage[100] = "";
uint16_t messageLength;
int16_t textPosition = displayWidth;
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
void setup()
{
    Serial.begin(9600);
    myDisplay.begin();
    pinMode(PIR_SENSOR1_PIN, INPUT);
    pinMode(PIR_SENSOR2_PIN, INPUT);

    pinMode(RESTART_BUTTON_PIN, INPUT_PULLUP);
    EEPROM.begin();
    myDisplay.setInvert(false);
    myDisplay.setIntensity(8); // Set the display intensity (0-15, lower value for dimmer display)

    messageLength = strlen(message) * charWidth; // Calculate the total width of the text

    myDisplay.displayReset(); // Reset the display to prepare for new content
    myDisplay.displayZoneText(0, message, PA_LEFT, 35, 0, PA_SCROLL_LEFT, PA_SCROLL_LEFT);
    myDisplay.displayAnimate();
    // Initialize the second display
    myDisplay2.begin();
    myDisplay2.setIntensity(8); // Set the brightness of the second display (0-15)
    if (read() >= 999)
    {
        Serial.println("score is greater than 999 formatting eeprom");
        write(0);
    }

    maxScore = read();
    // buf[19] = '\0';
    sprintf(maxMessage, "%d", maxScore);
    myDisplay2.displayClear();
    myDisplay2.displayText(maxMessage, PA_CENTER, 1000, 0, PA_PRINT);

    // Update display animations for the first display
    myDisplay2.displayAnimate();
    Serial.println("code started");
    Serial.println(maxMessage);
    mp3setup();
}

void loop()
{
    bool lossPlaying = true;
    bool wonPlaying = true;
    // myDisplay.displayReset();
    myDisplay2.displayText(maxMessage, PA_CENTER, 1000, 0, PA_PRINT);
    myDisplay2.displayAnimate();
    if (myDisplay.displayAnimate())
    {
        // If the scrolling animation is complete, restart it
        myDisplay.displayReset();
        myDisplay.displayZoneText(0, message, PA_LEFT, 35, 0, PA_SCROLL_LEFT, PA_SCROLL_LEFT);
        myDisplay.displayAnimate(); // Start the animation again
    }

    // Check if the first sensor detects movement
    if (digitalRead(PIR_SENSOR1_PIN) == LOW && firstSensorTime == 0)
    {
        firstSensorTime = millis(); // Record the time when first sensor detects movement
        Serial.println("First sensor detected movement");
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
            mappedScore = map(timeDifference, 0, 1000, 999, 1);
            // checkscore(mappedScore);
        }

        if (mappedScore > 999)
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
            sprintf(message, "%d", score);
            myDisplay.displayReset();
            myDisplay.displayZoneText(0, message, PA_CENTER, 35, 0, PA_PRINT, PA_PRINT);
            myDisplay.displayAnimate();

            if ((score + 30) >= mappedScore)
            {
                delay(100);
            }
            else if ((score + 10) >= mappedScore)
            {
                delay(600);
            }
            else if ((score + 7) >= mappedScore)
            {
                delay(1500);
            }
            else
            {
                delay(30);
            }
            //checkscore(score);
            if (score == mappedScore - 1 || score == mappedScore)
            {
                sprintf(message, "%d", mappedScore);
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

                        myDisplay.displayReset();

                        switch (currentText)
                        {
                        case 0:
                            myDisplay.displayZoneText(0, message, PA_CENTER, 35, 0, PA_PRINT, PA_PRINT);
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
                                myDisplay.displayZoneText(0, lossMessage, PA_CENTER, 35, 0, PA_PRINT, PA_PRINT);
                            }
                            else
                            {
                                if (wonPlaying)
                                {
                                    myDFPlayer.play(2);
                                    wonPlaying = false;
                                }
                                myDisplay.displayZoneText(0, wonMessage, PA_CENTER, 35, 0, PA_PRINT, PA_PRINT);
                            }
                            currentText = 0;
                            break;
                        }

                        myDisplay.displayAnimate();
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
        sprintf(maxMessage, "%d", maxScore);
        myDisplay2.displayReset();
        myDisplay2.displayText(maxMessage, PA_CENTER, 1000, 0, PA_PRINT);
        myDisplay2.displayAnimate();
    }
    else
    {
        myDisplay2.displayReset();
        maxScore = read();
        sprintf(maxMessage, "%d", maxScore);
        myDisplay2.displayText(maxMessage, PA_CENTER, 1000, 0, PA_PRINT);
        myDisplay2.displayAnimate();
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
    if (number == 0)
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