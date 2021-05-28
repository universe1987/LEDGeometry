#include "FlameEffect.h"

#include <FastLED.h>

#include "LEDCurve.h"
#include "Shape.h"

namespace LEDGeometry {
FlameEffect::FlameEffect(uint8_t* heat, uint8_t resolution, uint8_t* projection,
                         uint8_t cooling, uint8_t sparking)
    : heat(heat),
      resolution(resolution),
      projection(projection),
      cooling(cooling),
      sparking(sparking) {
    for (int i = 0; i < resolution; i++) {
        heat[i] = 0;
    }
}

FlameEffect::FlameEffect(uint8_t* heat, uint8_t resolution, uint8_t* projection)
    : FlameEffect(heat, resolution, projection,
                  (uint8_t)((550 / resolution) + 2), 120) {}

void FlameEffect::update_heat() {
    // randomly cool down
    for (int i = 0; i < resolution; i++) {
        heat[i] = qsub8(heat[i], random8(0, cooling));
    }
    // transmit heat up
    for (int i = resolution - 1; i >= 2; i--) {
        // this does not overflow, uint8_t is converted to int before addition
        heat[i] = (heat[i - 1] + heat[i - 2] + heat[i - 2]) / 3;
    }
    // random ignition
    if (random8() < sparking) {
        uint8_t y = random8(7);
        heat[y] = qadd8(heat[y], random8(160, 255));
    }
}

void FlameEffect::update(LEDCurve* led_curve) {
    for (int i = 0; i < led_curve->shape->n_points(); i++) {
        uint8_t temperature = heat[projection[i]];
        uint8_t color_index = scale8(temperature, 240);
        led_curve->leds[i] = ColorFromPalette(HeatColors_p, color_index);
    }
    update_heat();
}
}  // namespace LEDGeometry
