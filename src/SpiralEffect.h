#pragma once
#include <stdint.h>

#include "LightEffect.h"

namespace LEDGeometry {
class LEDCurve;
class SpiralEffect : public LightEffect {
   public:
    SpiralEffect(uint8_t n_segments, uint8_t segment_length);

   protected:
    void update(LEDCurve* led_curve);

   private:
    uint8_t n_segments;
    uint8_t segment_length;
    uint8_t pos;
};
}  // namespace LEDGeometry
