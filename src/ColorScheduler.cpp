#include "ColorScheduler.h"

#include <math.h>

namespace LEDGeometry {
ColorScheduler::ColorScheduler(uint16_t cycle, uint8_t hue,
                               uint8_t min_hue_delta, uint8_t max_hue_delta)
    : cycle(cycle),
      hue(hue),
      min_hue_delta(min_hue_delta),
      max_hue_delta(max_hue_delta),
      discrete_mode(false) {
    end_color.setHue(hue);
    change_theme();
    current_color = start_color;
}

ColorScheduler::ColorScheduler(uint16_t cycle)
    : ColorScheduler(cycle, random8(), 32, 224) {}

void ColorScheduler::set_cycle(uint16_t new_cycle) {
    cycle = new_cycle;
    progress = 0;
}

uint8_t ColorScheduler::get_progress() const {
    return (uint8_t)(progress * 255 / cycle);
}

void ColorScheduler::change_theme() {
    start_color = end_color;
    hue += random8(min_hue_delta, max_hue_delta);
    end_color.setHue(hue);
}

CRGB ColorScheduler::next_color() {
    if (discrete_mode) {
        current_color = start_color;
    } else {
        uint8_t frac = (uint8_t)(255 * progress / cycle);
        current_color = blend(start_color, end_color, frac);
    }
    ++progress;
    if (progress >= cycle) {
        progress = 0;
        change_theme();
    }
    return current_color;
}

void ColorScheduler::set_discrete_mode() { discrete_mode = true; }

void ColorScheduler::set_continuous_mode() { discrete_mode = false; }

void ColorScheduler::set_hue_deltas(uint8_t min_value, uint8_t max_value) {
    min_hue_delta = min_value;
    max_hue_delta = max_value;
}
}  // namespace LEDGeometry
