#pragma once
#include <stdint.h>

#include "LightEffect.h"

namespace LEDGeometry {
class LEDCurve;

class PulseEffect : public LightEffect {
   public:
    PulseEffect();
    PulseEffect(uint8_t beats_per_minute);

   protected:
    void update(LEDCurve* led_curve);

   private:
    uint8_t beats_per_minute;
};
}  // namespace LEDGeometry
