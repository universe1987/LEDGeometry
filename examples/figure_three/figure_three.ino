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
#define RESOLUTION 50

using namespace LEDGeometry;

/**
 * Create as global variables because dynamic allocation seems to be problematic for nano.
 * int16_t: 2 bytes, CRGB: 3 bytes, int, float: 4 bytes
 */
CRGB leds[NUM_LEDS];
CRGB color_buffer[COLOR_BUFFER_SIZE];
uint8_t byte_buffer[RESOLUTION + NUM_LEDS/2];

int16_t x_coords[60] = {5822, 5356, 4689, 4067, 3356, 2667, 1867, 1046, 312, -398, -1042, -1642, -2132, -2421, -2598, -2620, -2598, -2354, -2065, -1665, -1265, -799, -176, 512, 1179, 1357, 580, -109, -755, -1397, -2131, -2776, -3242, -3731, -4198, -4419, -4554, -4641, -4732, -4554, -4263, -3886, -3464, -2887, -2309, -1687, -999, -266, 468, 1312, 2069, 2826, 3555, 4255, 4845, 5311, 5800, 6378, 7146, 7999};
int16_t y_coords[60] = {-7754, -8204, -8569, -8904, -9154, -9253, -9287, -9303, -9170, -8953, -8636, -8253, -7803, -7220, -6703, -6088, -5554, -4988, -4437, -3921, -3405, -2955, -2571, -2272, -1921, -1431, -1064, -730, -381, -80, 204, 518, 969, 1469, 2019, 2519, 3068, 3652, 4267, 4917, 5434, 5950, 6435, 6900, 7334, 7667, 7983, 8216, 8434, 8634, 8585, 8344, 8183, 7941, 7584, 7067, 6583, 6185, 5902, 6001};
// Curve is a folded, it's shape has half the number of points
CustomShape figure_three = CustomShape(NUM_LEDS / 2, x_coords, y_coords);
// Color changes every 100 frames, this can be modified by calling set_cycle.
ColorScheduler color_scheduler = ColorScheduler(100);
LEDCurve my_light(leds, (Shape*)&figure_three, &color_scheduler, true);

MonoColorEffect mono;
SignalTransmissionEffect transmission;
PulseEffect pulse;
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
  radial_projection((Shape*)&figure_three, byte_buffer, COLOR_BUFFER_SIZE);
  my_light.set_effect((LightEffect*)&ripple, 20, duration);
  // 4. Pulse effect 16fps for 30s.
  color_scheduler.set_cycle(120);
  my_light.set_effect((LightEffect*)&pulse, 16, duration);
  // 5. Tide effect 20fps for 30s, projection direction is randomly chosen.
  parallel_projection((Shape*)&figure_three, byte_buffer, COLOR_BUFFER_SIZE);
  my_light.set_effect((LightEffect*)&tide, 20, duration);
  // 6. Flame effect 30 fps for 30s.
  parallel_projection((Shape*)&figure_three, byte_buffer + RESOLUTION, RESOLUTION, 0.0, 1.0);
  my_light.set_effect((LightEffect*)&flame, 30, duration);
}
