#pragma once
#include <stdint.h>

class CRGB;
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
    LEDCurve(CRGB* leds, Shape* shape, ColorScheduler* color_scheduler,
             bool folded);
    void set_blackout(uint8_t n_blackouts, uint8_t* blackout);
    // Pass control to the light effect.
    void set_effect(LightEffect* effect, int fps, int n_seconds);

   private:
    bool folded;
    // Handle folded curve and display the assigned color of the LEDs.
    void display(int sleep_ms);
};
}  // namespace LEDGeometry
