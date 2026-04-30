#include "SpiralEffect.h"

#include "ColorScheduler.h"
#include "LEDCurve.h"
#include "Shape.h"
#include "Type.h"

namespace LEDGeometry {
SpiralEffect::SpiralEffect(uint8_t n_segments, uint8_t segment_length)
    : n_segments(n_segments), segment_length(segment_length), pos(0) {}

void SpiralEffect::update(LEDCurve* led_curve) {
    CRGB color = led_curve->color_scheduler->next_color();
    int n = led_curve->shape->n_points();
    for (int i = 0; i < n_segments; i++) {
        int idx = pos + i * segment_length;
        if (idx < n) {
            led_curve->leds[idx] = color;
        }
    }
    ++pos;
    if (pos >= segment_length) {
        pos = 0;
    }
}
}  // namespace LEDGeometry
