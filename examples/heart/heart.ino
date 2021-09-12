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
#define BRIGHTNESS  144

#define COLOR_BUFFER_SIZE 32
#define RESOLUTION 38

using namespace LEDGeometry;

/**
 * Create as global variables because dynamic allocation seems to be problematic for nano.
 * int16_t: 2 bytes, CRGB: 3 bytes, int, float: 4 bytes
 */
CRGB leds[NUM_LEDS];
CRGB color_buffer[COLOR_BUFFER_SIZE];
uint8_t byte_buffer[RESOLUTION + NUM_LEDS/2];

int16_t x_coords[60] = {771, 1752, 2466, 3154, 3890, 4620, 5343, 6052, 6737, 7402, 8059, 8658, 9177, 9535, 9637, 9564, 9410, 9008, 8293, 7504, 6606, 5525, 4358, 3452, 2554, 1795, 1232, 808, 372, -337, -819, -1126, -1608, -2170, -2950, -3967, -5040, -6142, -7223, -8143, -8887, -9493, -9705, -9779, -9727, -9406, -8909, -8333, -7646, -6937, -6265, -5485, -4761, -4148, -3426, -2579, -1805, -1089, -279, 451};
int16_t y_coords[60] = {6670, 6139, 5673, 5219, 4715, 4260, 3767, 3252, 2733, 2157, 1544, 947, 285, -433, -1205, -1993, -2766, -3461, -4064, -4573, -4956, -5131, -5045, -4634, -4074, -3422, -2699, -1917, -1188, -1019, -1801, -2546, -3319, -4090, -4737, -5126, -5203, -5087, -4868, -4386, -3811, -3144, -2371, -1539, -756, -22, 679, 1352, 1938, 2546, 3077, 3570, 4091, 4638, 5077, 5421, 5860, 6232, 6560, 6987};
// Curve is a folded, it's shape has half the number of points
CustomShape heart_shape = CustomShape(NUM_LEDS / 2, x_coords, y_coords);
// Color changes every 100 frames, this can be modified by calling set_cycle.
ColorScheduler color_scheduler = ColorScheduler(100);
LEDCurve my_light(leds, (Shape*)&heart_shape, &color_scheduler, true);

MonoColorEffect mono;
SignalTransmissionEffect transmission;
PulseEffect pulse;
SpiralEffect spiral(2, NUM_LEDS / 4);
FlameEffect flame(byte_buffer, RESOLUTION, byte_buffer + RESOLUTION);
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
  // 2. Signal transmission effect 20fps for 30s.
  my_light.set_effect((LightEffect*)&transmission, 20, duration);
  // 3. Ripple effect 20fps for 30s, center is randomly chosen.
  radial_projection((Shape*)&heart_shape, byte_buffer, COLOR_BUFFER_SIZE);
  my_light.set_effect((LightEffect*)&ripple, 20, duration);
  // 4. Pulse effect 16fps for 30s.
  color_scheduler.set_cycle(120);
  my_light.set_effect((LightEffect*)&pulse, 16, duration);
  // 5. Tide effect 20fps for 30s, projection direction is randomly chosen.
  parallel_projection((Shape*)&heart_shape, byte_buffer, COLOR_BUFFER_SIZE);
  my_light.set_effect((LightEffect*)&tide, 20, duration);
  // 6. Spiral effect 15 fps for 30s, looks better under discrete mode.
  color_scheduler.set_discrete_mode();
  color_scheduler.set_cycle(NUM_LEDS / 4);
  my_light.set_effect((LightEffect*)&spiral, 15, duration);
  // 7. Flame effect 30 fps for 30s.
  intrinsic_projection((Shape*)&heart_shape, byte_buffer + RESOLUTION, RESOLUTION);
  my_light.set_effect((LightEffect*)&flame, 30, duration);
}
