#pragma once
#include <stdint.h>

struct CRGB;
namespace LEDGeometry {
class Shape;
class ColorScheduler;
class LightEffect;

class LEDCurve {
   public:
    CRGB* leds;
    Shape* shape;
    ColorScheduler* color_scheduler;
    uint8_t* blackout;
    uint8_t n_blackouts;
    // When folded=true, leds[] must have capacity for 2 * shape->n_points()
    // entries; display() mirrors the first half into the second half each frame.
    LEDCurve(CRGB* leds, Shape* shape, ColorScheduler* color_scheduler,
             bool folded);
    void set_blackout(uint8_t n_blackouts, uint8_t* blackout);
    // Pass control to the light effect.
    void set_effect(LightEffect* effect, int fps, int n_seconds);

   private:
    bool folded;
    int frame_index;
    int effect_index;
    // Handle folded curve and display the assigned color of the LEDs.
    void display(int sleep_ms);
};
}  // namespace LEDGeometry
