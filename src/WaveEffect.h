#pragma once
#include <stdint.h>

#include "LightEffect.h"

class CRGB;

namespace LEDGeometry {
class LEDCurve;

class WaveEffect : public LightEffect {
   public:
    WaveEffect(CRGB* spectrum, uint8_t resolution, uint8_t* projection,
               uint8_t frequency);

   protected:
    void update(LEDCurve* led_curve);
    void propagate();

   private:
    CRGB* spectrum;
    uint8_t resolution;
    uint8_t* projection;
    uint8_t frequency;
    uint8_t phase;
};
}  // namespace LEDGeometry
