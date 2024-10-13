#include "SpiralEffect.h"

#include <FastLED.h>

#include "ColorScheduler.h"
#include "LEDCurve.h"

namespace LEDGeometry {
SpiralEffect::SpiralEffect(uint8_t n_segments, uint8_t segment_length)
    : pos(0), n_segments(n_segments), segment_length(segment_length) {}

void SpiralEffect::update(LEDCurve* led_curve) {
    CRGB color = led_curve->color_scheduler->next_color();
    for (int i = 0; i < n_segments; i++) {
        led_curve->leds[pos + i * segment_length] = color;
    }
    ++pos;
    if (pos >= segment_length) {
        pos = 0;
    }
}
}  // namespace LEDGeometry
