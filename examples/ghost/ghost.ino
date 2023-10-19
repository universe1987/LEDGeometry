#include "BlinkEffect2.h"
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

int16_t x_coords[120] = {-5898, -5402, -4993, -4489, -3892, -3295, -2689, -2192, -1718, -1147, -401, 309, 933, 1569, 2082, 2465, 2829, 3249, 3714, 4499, 5114, 5834, 6450, 6954, 7224, 7420, 7522, 7534, 7515, 7505, 7421, 7298, 7169, 7018, 6833, 6646, 6402, 6141, 5787, 5499, 5126, 4714, 4219, 3725, 3117, 2483, 1857, 1251, 505, -159, -803, -1446, -2110, -2753, -3350, -3966, -4508, -4993, -5404, -5740, -6067, -6263, -6310, -6243, -6011, -5526, -4955, -4349, -3883, -3424, -2976, -2408, -1632, -980, -196, 439, 962, 1437, 1952, 2604, 3315, 3688, 3631, 3314, 2790, 2260, 1746, 1316, 1007, 880, 933, 1185, 1503, 1661, 1429, 803, 375, 262, 291, 234, 74, -327, -867, -1549, -2128, -2865, -3547, -4163, -4854, -5423, -5861, -6281, -6552, -6720, -6832, -6859, -6832, -6710, -6609, -6468};
int16_t y_coords[120] = {7265, 7975, 8487, 8936, 9197, 9272, 9084, 8648, 8087, 7675, 7675, 7939, 8142, 8171, 7983, 7573, 6987, 6324, 5921, 5922, 6060, 6202, 6089, 5716, 5230, 4585, 3895, 3178, 2533, 1815, 1142, 433, -267, -837, -1592, -2283, -2964, -3591, -4272, -4849, -5410, -5943, -6493, -6913, -7351, -7688, -8052, -8304, -8527, -8631, -8658, -8620, -8509, -8239, -7891, -7473, -7034, -6484, -5943, -5326, -4730, -4057, -3413, -2778, -2144, -1553, -1407, -1574, -2032, -2591, -3217, -3684, -3665, -3497, -3320, -3356, -3702, -4178, -4711, -5065, -4905, -4271, -3553, -2872, -2284, -1760, -1258, -697, -44, 537, 1225, 1860, 2514, 3253, 3869, 3923, 3419, 2626, 1946, 1300, 769, 143, -389, -791, -1024, -1126, -1126, -1005, -874, -482, 13, 610, 1217, 1963, 2682, 3290, 3961, 4660, 5332, 6023};

CustomShape my_shape = CustomShape(NUM_LEDS, x_coords, y_coords);
// Color changes every 100 frames, this can be modified by calling set_cycle.
ColorScheduler color_scheduler = ColorScheduler(100);
LEDCurve my_light(leds, (Shape*)&my_shape, &color_scheduler, FOLDED);
uint8_t blackout[23] = {66, 67, 68, 74, 75, 81, 82, 83, 84, 85, 86, 87, 88, 89, 100, 101, 102, 103, 104, 105, 106, 107, 108};

MonoColorEffect mono;
SignalTransmissionEffect transmission;
PulseEffect pulse;
FlameEffect flame(byte_buffer, RESOLUTION + 8, byte_buffer + RESOLUTION + 8);
WaveEffect tide(color_buffer, COLOR_BUFFER_SIZE, byte_buffer, 8);
BlinkEffect2 bblink;

void setup() {
  delay(3000); // sanity delay
  FastLED.addLeds<CHIPSET, LED_PIN, COLOR_ORDER>(leds, NUM_LEDS).setCorrection(TypicalLEDStrip).setTemperature(HighNoonSun);
  FastLED.setBrightness(BRIGHTNESS);
  my_light.set_blackout(23, (uint8_t*)&blackout);
}

void loop() {
  random16_add_entropy(random());
  int duration = 30;
  // 1. Mono color effect 10fps for 30s.
  color_scheduler.set_cycle(100);
  color_scheduler.set_continuous_mode();
  my_light.set_effect((LightEffect*)&mono, 10, duration);
  // 2. Blink effect 40fps for 30s, center is randomly chosen.
  my_light.set_effect((BlinkEffect2*)&bblink, 20, duration);
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
