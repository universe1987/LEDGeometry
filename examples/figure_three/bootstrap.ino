#include <FastLED.h>

#define LED_PIN     7
#define COLOR_ORDER GRB
#define CHIPSET     WS2812B
#define NUM_LEDS    120
#define BRIGHTNESS  144

CRGB leds[NUM_LEDS];   

void setup() {
  delay(3000); // sanity delay
  FastLED.addLeds<CHIPSET, LED_PIN, COLOR_ORDER>(leds, NUM_LEDS).setCorrection(TypicalLEDStrip).setTemperature(HighNoonSun);
  FastLED.setBrightness(BRIGHTNESS);
  for (int i = 0; i < NUM_LEDS; i++) {
    leds[i] = CRGB::Green;
  }
  FastLED.show();
}
