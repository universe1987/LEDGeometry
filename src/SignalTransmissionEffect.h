#pragma once
#include <stdint.h>

#include "LightEffect.h"

namespace LEDGeometry {
class LEDCurve;

class SignalTransmissionEffect : public LightEffect {
   public:
    SignalTransmissionEffect();
    SignalTransmissionEffect(uint8_t fade_factor);

   protected:
    void update(LEDCurve* led_curve);

   private:
    uint8_t fade_factor;
    uint8_t pos;
};
}  // namespace LEDGeometry
