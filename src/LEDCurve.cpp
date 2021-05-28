#include "LEDCurve.h"

#include <FastLED.h>
#include <stdint.h>

#include "ColorScheduler.h"
#include "LightEffect.h"
#include "Shape.h"

namespace LEDGeometry {
LEDCurve::LEDCurve(CRGB* leds, Shape* shape, ColorScheduler* color_scheduler,
                   bool folded)
    : leds(leds),
      shape(shape),
      color_scheduler(color_scheduler),
      folded(folded){};

void LEDCurve::display(int sleep_ms) {
    if (folded) {
        uint8_t n = shape->n_points();
        for (int i = 0; i < n; i++) {
            leds[n + i] = leds[n - i - 1];
        }
    }
    FastLED.show();
    FastLED.delay(sleep_ms);
}

void LEDCurve::set_effect(LightEffect* effect, int fps, int n_seconds) {
    int sleep_ms = 1000 / fps;
    int n_iters = n_seconds * fps;
    for (int i = 0; i < n_iters; i++) {
        effect->update(this);
        display(sleep_ms);
    }
}
}  // namespace LEDGeometry
