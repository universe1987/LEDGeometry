#pragma once
#include "LightEffect.h"

namespace LEDGeometry {
class LEDCurve;

class MonoColorEffect : public LightEffect {
   protected:
    void update(LEDCurve* led_curve);
};
}  // namespace LEDGeometry
