#pragma once
#include <FastLED.h>
#include <stdint.h>

namespace LEDGeometry {
/**
 * Manages the flow of color, it returns a new color each time next_color() is called.
 * ColorScheduler works in two modes: continusous and discrete.
 * 
 * [Continuous mode] within each cycle, current_color changes from start_color to end_color 
 * with constant speed, use continuous mode for gradually changing colors.
 * [Discrete mode] within each cycle, current_color equals start_color.
 * Starting from the next cycle, the end_color becomes the start_color, and change_theme method
 * generates a new end_color.
 *
 * @param cycle Number of frames to completely change to the next color.
 * @param hue The initial hue of the theme color, the theme color always has maximum sat and val.
 * @param min_hue_delta The minimum increment to the hue of the next theme color.
 * @param max_hue_delta The maximum increment to the hue of the next theme color.
 */
class ColorScheduler {
   public:
    ColorScheduler(uint16_t cycle, uint8_t hue, uint8_t min_hue_delta,
                   uint8_t max_hue_delta);
    ColorScheduler(uint16_t cycle);
    // Returns a color and increment the state.
    CRGB next_color();
    // Set a new cycle and reset progress to 0.
    void set_cycle(uint16_t new_cycle);
    // Returns the progress with respect to the cycle, scaled to 0 ~ 255.
    uint8_t get_progress() const;
    // In discrete mode, next_color() always returns the same color until the next cycle starts.
    void set_discrete_mode();
    // In continuous mode, next_color() returns a color interpolated between start_color and
    // end_color, so that the transi
    void set_continuous_mode();
    void set_hue_deltas(uint8_t min_value, uint8_t max_value);

   protected:
    // Change to a new theme at the end of a cycle.
    virtual void change_theme();

   private:
    uint16_t cycle;
    uint8_t hue;
    uint8_t min_hue_delta;
    uint8_t max_hue_delta;
    uint16_t progress;
    bool discrete_mode;
    CRGB start_color;
    CRGB end_color;
    CRGB current_color;
};
}  // namespace LEDGeometry
