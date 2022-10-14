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
#define RESOLUTION 40

using namespace LEDGeometry;

/**
 * Create as global variables because dynamic allocation seems to be problematic for nano.
 * int16_t: 2 bytes, CRGB: 3 bytes, int, float: 4 bytes
 */
CRGB leds[NUM_LEDS];
CRGB color_buffer[COLOR_BUFFER_SIZE];
uint8_t byte_buffer[RESOLUTION + NUM_LEDS];

int16_t x_coords[120] = {8625, 7812, 7231, 6843, 6049, 5526, 4904, 4325, 3608, 2969, 2369, 1652, 799, 276, -363, -1080, -1758, -2608, -3061, -3656, -4251, -5031, -5225, -5651, -6252, -6910, -7278, -7724, -8131, -8441, -8673, -8898, -9041, -9099, -8809, -8652, -8499, -8378, -7821, -7491, -7088, -6813, -6271, -6059, -5490, -5129, -4605, -3748, -3465, -2881, -2377, -1758, -1199, -246, 487, 993, 1537, 2142, 2717, 3085, 3976, 4383, 4790, 5203, 5474, 6282, 6650, 7227, 7459, 8006, 8513, 8742, 9207, 9613, 9632, 9052, 8315, 7366, 6481, 5662, 4790, 4054, 3821, 4329, 4831, 5387, 5875, 6547, 6824, 6857, 6909, 6572, 5952, 5158, 4364, 3511, 2543, 1807, 1303, 763, 238, -460, -1118, -1680, -2197, -2751, -3501, -4373, -4992, -5457, -5980, -6542, -6780, -6833, -6775, -6600, -6193, -5767, -5438, -5070};
int16_t y_coords[120] = {3784, 3623, 4177, 3817, 3745, 4520, 4735, 4017, 4695, 5043, 4578, 3958, 4346, 5057, 5431, 4985, 4326, 4542, 5121, 5121, 4423, 4249, 4869, 4927, 4249, 4288, 4578, 3997, 3242, 2447, 1634, 765, -90, -923, -768, -192, 568, 1518, 1711, 1033, 1403, 2058, 2157, 1268, 1337, 1960, 2661, 2547, 1645, 1144, 1324, 2060, 2836, 3371, 2716, 1892, 1114, 704, 1266, 2138, 2254, 1324, 607, 1014, 1944, 1828, 917, 433, 979, 1537, 588, -278, -981, -1853, -2689, -3195, -3344, -3267, -2976, -2976, -3209, -3732, -4487, -5223, -5958, -6669, -7432, -7316, -6579, -5650, -4736, -3945, -3403, -3286, -3480, -3635, -3461, -2860, -2163, -1463, -826, -555, -942, -1601, -2357, -3131, -3713, -3848, -3519, -3170, -3035, -3364, -4061, -4952, -5824, -6696, -7102, -6696, -6037, -5495};

CustomShape pumpkin_shape = CustomShape(NUM_LEDS, x_coords, y_coords);
// Color changes every 100 frames, this can be modified by calling set_cycle.
ColorScheduler color_scheduler = ColorScheduler(100);
LEDCurve my_light(leds, (Shape*)&pumpkin_shape, &color_scheduler, true);

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
  // 2. Ripple effect 20fps for 30s, center is randomly chosen.
  radial_projection((Shape*)&pumpkin_shape, byte_buffer, COLOR_BUFFER_SIZE);
  my_light.set_effect((LightEffect*)&ripple, 20, duration);
  // 3. Pulse effect 16fps for 30s.
  color_scheduler.set_cycle(120);
  my_light.set_effect((LightEffect*)&pulse, 16, duration);
  // 4. Tide effect 20fps for 30s, projection direction is randomly chosen.
  parallel_projection((Shape*)&pumpkin_shape, byte_buffer, COLOR_BUFFER_SIZE);
  my_light.set_effect((LightEffect*)&tide, 20, duration);
  // 5. Flame effect 30 fps for 30s.
  color_scheduler.set_cycle(450);
  intrinsic_projection((Shape*)&pumpkin_shape, byte_buffer + RESOLUTION, RESOLUTION);
  flame.set_random_haunt_mode();
  my_light.set_effect((LightEffect*)&flame, 30, duration);
}
