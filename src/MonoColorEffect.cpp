#include "MonoColorEffect.h"

#include <stdint.h>

#include "ColorScheduler.h"
#include "LEDCurve.h"
#include "Shape.h"
#include "Type.h"

namespace LEDGeometry {
void MonoColorEffect::update(LEDCurve* led_curve) {
    CRGB color = led_curve->color_scheduler->next_color();
    for (int i = 0; i < led_curve->shape->n_points(); i++) {
        led_curve->leds[i] = color;
    }
}
}  // namespace LEDGeometry
