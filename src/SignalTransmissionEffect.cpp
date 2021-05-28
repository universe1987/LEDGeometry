#include "SignalTransmissionEffect.h"

#include <FastLED.h>

#include "ColorScheduler.h"
#include "LEDCurve.h"
#include "Shape.h"

namespace LEDGeometry {
SignalTransmissionEffect::SignalTransmissionEffect(uint8_t fade_factor)
    : fade_factor(fade_factor), pos(0) {}

SignalTransmissionEffect::SignalTransmissionEffect()
    : SignalTransmissionEffect(16) {}

void SignalTransmissionEffect::update(LEDCurve* led_curve) {
    uint8_t n_points = led_curve->shape->n_points();
    for (int i = 0; i < n_points; i++) {
        led_curve->leds[i].fadeLightBy(fade_factor);
    }
    ++pos;
    if (pos >= n_points) {
        pos = 0;
    }
    led_curve->leds[pos] = led_curve->color_scheduler->next_color();
}
}  // namespace LEDGeometry
