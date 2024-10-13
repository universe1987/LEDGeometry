#include "PulseEffect.h"

#include "ColorScheduler.h"
#include "LEDCurve.h"
#include "Shape.h"
#include "Type.h"

namespace LEDGeometry {

PulseEffect::PulseEffect() : PulseEffect(30) {}

PulseEffect::PulseEffect(uint8_t beats_per_minute)
    : beats_per_minute(beats_per_minute) {}

void PulseEffect::update(LEDCurve* led_curve) {
    CRGB color = led_curve->color_scheduler->next_color();
    color.fadeLightBy(beatsin8(beats_per_minute));
    for (int i = 0; i < led_curve->shape->n_points(); i++) {
        led_curve->leds[i] = color;
    }
}
}  // namespace LEDGeometry
