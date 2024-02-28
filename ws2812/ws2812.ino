#include <FastLED.h>

#define LED_PIN 7
#define NUM_LEDS 10
#define LED_TYPE WS2812
#define COLOR_ORDER GRB

CRGB leds[NUM_LEDS];

void setup()
{
    FastLED.addLeds<LED_TYPE, LED_PIN, COLOR_ORDER>(leds, NUM_LEDS);
}

void loop()
{
    // Blink all LEDs
    colorWipe(CRGB::Red, 50);   // Red
    colorWipe(CRGB::Black, 50); // Off

    // Animate all LEDs
    rainbow(20);
}

void colorWipe(CRGB color, int wait)
{
    for (int i = 0; i < NUM_LEDS; i++)
    {
        leds[i] = color;
        FastLED.show();
        delay(wait);
    }
}

void rainbow(int wait)
{
    // Hue rotation
    for (int j = 0; j < 256; j++)
    {
        for (int i = 0; i < NUM_LEDS; i++)
        {
            leds[i] = CHSV(j, 255, 255);
        }
        FastLED.show();
        delay(wait);
    }
}
