#include "ColorScheduler.h"
#include "CustomShape.h"
#include "FlameEffect.h"
#include "LEDCurve.h"
#include "LightEffect.h"
#include "MonoColorEffect.h"
#include "Projection.h"
#include "PulseEffect.h"
#include "SignalTransmissionEffect.h"
#include "SpiralEffect.h"
#include "WaveEffect.h"

#include <FastLED.h>

#define LED_PIN     7
#define COLOR_ORDER GRB
#define CHIPSET     WS2812B
#define NUM_LEDS    120
#define BRIGHTNESS  120

#define COLOR_BUFFER_SIZE 32
#define RESOLUTION 50
#define FOLDED false

using namespace LEDGeometry;

/**
 * Create as global variables because dynamic allocation seems to be problematic for nano.
 * int16_t: 2 bytes, CRGB: 3 bytes, int, float: 4 bytes
 */
CRGB leds[NUM_LEDS];
CRGB color_buffer[COLOR_BUFFER_SIZE];
uint8_t byte_buffer[RESOLUTION + NUM_LEDS + 8];

{{x_coords}}
{{y_coords}}

CustomShape my_shape = CustomShape(NUM_LEDS, x_coords, y_coords);
// Color changes every 100 frames, this can be modified by calling set_cycle.
ColorScheduler color_scheduler = ColorScheduler(100);
LEDCurve my_light(leds, (Shape*)&my_shape, &color_scheduler, FOLDED);

MonoColorEffect mono;
SignalTransmissionEffect transmission;
PulseEffect pulse;
FlameEffect flame(byte_buffer, RESOLUTION + 8, byte_buffer + RESOLUTION + 8);
WaveEffect ripple(color_buffer, COLOR_BUFFER_SIZE, byte_buffer, 8);
WaveEffect tide(color_buffer, COLOR_BUFFER_SIZE, byte_buffer, 8);

void setup() {
  delay(3000); // sanity delay
  FastLED.addLeds<CHIPSET, LED_PIN, COLOR_ORDER>(leds, NUM_LEDS).setCorrection(TypicalLEDStrip).setTemperature(HighNoonSun);
  FastLED.setBrightness(BRIGHTNESS);
}

void loop() {
  random16_add_entropy(random());
  int duration = 30;
  // 1. Mono color effect 10fps for 30s.
  color_scheduler.set_cycle(100);
  color_scheduler.set_continuous_mode();
  my_light.set_effect((LightEffect*)&mono, 10, duration);
  // 2. Ripple effect 20fps for 30s, center is randomly chosen.
  radial_projection((Shape*)&my_shape, byte_buffer, COLOR_BUFFER_SIZE);
  my_light.set_effect((LightEffect*)&ripple, 20, duration);
  // 3. Pulse effect 16fps for 30s.
  color_scheduler.set_cycle(120);
  my_light.set_effect((LightEffect*)&pulse, 16, duration);
  // 4. Tide effect 20fps for 30s, projection direction is randomly chosen.
  parallel_projection((Shape*)&my_shape, byte_buffer, COLOR_BUFFER_SIZE);
  my_light.set_effect((LightEffect*)&tide, 20, duration);
  // 5. Flame effect 30 fps for 30s.
  color_scheduler.set_cycle(600);
  parallel_projection((Shape*)&my_shape, byte_buffer + RESOLUTION + 8, RESOLUTION, 0, -1);
  my_light.set_effect((LightEffect*)&flame, 50, duration);
}
