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

int16_t x_coords[120] = {-3486, -4030, -4642, -5122, -5602, -6101, -6557, -7101, -7587, -7963, -8166, -8223, -8057, -7807, -7469, -7180, -6802, -6411, -6102, -5705, -5387, -5117, -4867, -4652, -4696, -4907, -5494, -6063, -6592, -7121, -7522, -7591, -7111, -6631, -6136, -5622, -5049, -4412, -3863, -3541, -3515, -3574, -3613, -3672, -3604, -3408, -3124, -2771, -2374, -1933, -1448, -929, -371, 178, 727, 1275, 1726, 2152, 2588, 2953, 3284, 3460, 3607, 3710, 3802, 3896, 4059, 4418, 5052, 5591, 6007, 6610, 7124, 7650, 7830, 7629, 7196, 6717, 6184, 5611, 5072, 4562, 4167, 4136, 4140, 4264, 4430, 4646, 4858, 5111, 5485, 5811, 6164, 6541, 7007, 7389, 7693, 7798, 7609, 7154, 6683, 6105, 5625, 5110, 4610, 4087, 3622, 3234, 2823, 2372, 1814, 1359, 819, 364, -184, -635, -1012, -1311, -1679, -2052};
int16_t y_coords[120] = {5711, 5838, 5944, 6039, 6179, 6308, 6387, 6427, 6323, 6049, 5588, 5113, 4599, 4172, 3722, 3379, 2977, 2585, 2213, 1782, 1356, 861, 446, -148, -756, -1226, -1555, -1701, -1765, -1917, -2264, -2798, -3244, -3313, -3146, -2935, -2720, -2725, -2925, -3366, -4010, -4606, -5135, -5698, -6266, -6737, -7197, -7602, -8010, -8412, -8770, -9104, -9343, -9450, -9397, -9210, -8921, -8559, -8143, -7661, -7145, -6634, -6105, -5512, -4963, -4434, -3847, -3415, -3322, -3567, -3783, -4042, -4090, -3847, -3368, -2882, -2563, -2444, -2407, -2406, -2362, -2117, -1623, -1110, -525, -46, 523, 1006, 1489, 1983, 2453, 2830, 3178, 3482, 3800, 4133, 4564, 5039, 5466, 5706, 5813, 5844, 5882, 5904, 5941, 6098, 6460, 6813, 7136, 7361, 7552, 7626, 7665, 7631, 7489, 7196, 6813, 6506, 6132, 5784};

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
