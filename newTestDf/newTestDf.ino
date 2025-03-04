#include "SoftwareSerial.h"
#include "DFRobotDFPlayerMini.h"

int currentTrack = 100;
bool pauseRequested = false;
bool stopRequested = false;

const byte busyPin = 10;
const byte ssRXPin = 5;
const byte ssTXPin = 6;

SoftwareSerial mp3ss(ssRXPin, ssTXPin); // RX, TX
DFRobotDFPlayerMini myDFPlayer;

void setup()
{
    Serial.begin(115200);
    mp3ss.begin(9600);
    myDFPlayer.begin(mp3ss);
    myDFPlayer.volume(25);
    pinMode(busyPin, INPUT);
}

// Core DFPlayer functions
void playTrack(int trackNumber)
{
    myDFPlayer.play(trackNumber);
}

void stopTrack()
{
    myDFPlayer.stop();
}

void pauseTrack()
{
    myDFPlayer.pause();
}

void resumeTrack()
{
    myDFPlayer.start();
}

void setVolume(int volume)
{
    myDFPlayer.volume(volume); // 0-30
}

bool isPlaying()
{
    return (digitalRead(busyPin) == 0);
}
void loop()
{
    static unsigned long timer = 0;
    unsigned long interval = 500;

    if (millis() - timer >= interval)
    {
        timer = millis();

        bool busyPinState = digitalRead(busyPin);
        if (busyPinState == 1)
        { // Track finished playing
            if (currentTrack >= 100 && currentTrack < 298)
            {
                myDFPlayer.stop();
                delay(500);

                // Skip tracks ending in 8 or 9
                if (currentTrack % 100 != 8 && currentTrack % 100 != 9)
                {
                    Serial.print("Playing track: ");
                    Serial.println(currentTrack);
                    myDFPlayer.play(currentTrack);
                    delay(500);
                }

                currentTrack++;

                // Reset to 100 if we reach end
                if (currentTrack >= 298)
                {
                    currentTrack = 100;
                }
            }
        }
    }

    // Handle pause/resume
    if (pauseRequested)
    {
        myDFPlayer.pause();
    }
    else
    {
        myDFPlayer.start();
    }

    // Handle stop
    if (stopRequested)
    {
        myDFPlayer.stop();
        currentTrack = 100; // Reset to beginning
    }
}
