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

int16_t x_coords[120] = {-2052, -1679, -1311, -1012, -635, -184, 364, 819, 1359, 1814, 2372, 2823, 3234, 3622, 4087, 4610, 5110, 5625, 6105, 6683, 7154, 7609, 7798, 7693, 7389, 7007, 6541, 6164, 5811, 5485, 5111, 4858, 4646, 4430, 4264, 4140, 4136, 4167, 4562, 5072, 5611, 6184, 6717, 7196, 7629, 7830, 7650, 7124, 6610, 6007, 5591, 5052, 4418, 4059, 3896, 3802, 3710, 3607, 3460, 3284, 2953, 2588, 2152, 1726, 1275, 727, 178, -371, -929, -1448, -1933, -2374, -2771, -3124, -3408, -3604, -3672, -3613, -3574, -3515, -3541, -3863, -4412, -5049, -5622, -6136, -6631, -7111, -7591, -7522, -7121, -6592, -6063, -5494, -4907, -4696, -4652, -4867, -5117, -5387, -5705, -6102, -6411, -6802, -7180, -7469, -7807, -8057, -8223, -8166, -7963, -7587, -7101, -6557, -6101, -5602, -5122, -4642, -4030, -3486};
int16_t y_coords[120] = {5784, 6132, 6506, 6813, 7196, 7489, 7631, 7665, 7626, 7552, 7361, 7136, 6813, 6460, 6098, 5941, 5904, 5882, 5844, 5813, 5706, 5466, 5039, 4564, 4133, 3800, 3482, 3178, 2830, 2453, 1983, 1489, 1006, 523, -46, -525, -1110, -1623, -2117, -2362, -2406, -2407, -2444, -2563, -2882, -3368, -3847, -4090, -4042, -3783, -3567, -3322, -3415, -3847, -4434, -4963, -5512, -6105, -6634, -7145, -7661, -8143, -8559, -8921, -9210, -9397, -9450, -9343, -9104, -8770, -8412, -8010, -7602, -7197, -6737, -6266, -5698, -5135, -4606, -4010, -3366, -2925, -2725, -2720, -2935, -3146, -3313, -3244, -2798, -2264, -1917, -1765, -1701, -1555, -1226, -756, -148, 446, 861, 1356, 1782, 2213, 2585, 2977, 3379, 3722, 4172, 4599, 5113, 5588, 6049, 6323, 6427, 6387, 6308, 6179, 6039, 5944, 5838, 5711};

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
