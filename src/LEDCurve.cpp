#include "LEDCurve.h"

#include <stdint.h>

#include "ColorScheduler.h"
#include "LightEffect.h"
#include "Shape.h"
#include "Type.h"

namespace LEDGeometry {
LEDCurve::LEDCurve(CRGB* leds, Shape* shape, ColorScheduler* color_scheduler,
                   bool folded)
    : leds(leds),
      shape(shape),
      color_scheduler(color_scheduler),
      blackout(nullptr),
      n_blackouts(0),
      folded(folded),
      frame_index(0){};

void LEDCurve::set_blackout(uint8_t n_blackouts, uint8_t* blackout) {
    this->n_blackouts = n_blackouts;
    this->blackout = blackout;
}

void LEDCurve::display(int sleep_ms) {
    if (folded) {
        uint8_t n = shape->n_points();
        for (int i = 0; i < n; i++) {
            leds[n + i] = leds[n - i - 1];
        }
    }
    for (int i = 0; i < n_blackouts; i++) {
        leds[blackout[i]] = CRGB::Black;
    }
#ifdef DEBUG
    int n_leds = folded ? 2 * shape->n_points() : shape->n_points();
    std::cout << "Frame " << frame_index;
    for (int i = 0; i < n_leds; i++) {
        std::cout << " (" << (int)leds[i].r << "," << (int)leds[i].g << "," << (int)leds[i].b << ")";
    }
    std::cout << std::endl;
    frame_index++;
#endif
    FastLED.show();
    FastLED.delay(sleep_ms);
}

void LEDCurve::set_effect(LightEffect* effect, int fps, int n_seconds) {
    frame_index = 0;
    int sleep_ms = 1000 / fps;
    int n_iters = n_seconds * fps;
    for (int i = 0; i < n_iters; i++) {
        effect->update(this);
        display(sleep_ms);
    }
}
}  // namespace LEDGeometry
