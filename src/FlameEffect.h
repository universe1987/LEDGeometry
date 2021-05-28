#pragma once
#include <stdint.h>

#include "LightEffect.h"

namespace LEDGeometry {
class LEDCurve;
// Adapted from Fire2012 by Mark Kriegsman, July 2012, as part of "Five
// Elements" shown here: http://youtu.be/knWiGsmgycY
class FlameEffect : public LightEffect {
   public:
    FlameEffect(uint8_t* heat, uint8_t resolution, uint8_t* projection);
    FlameEffect(uint8_t* heat, uint8_t resolution, uint8_t* projection,
                uint8_t cooling, uint8_t sparking);

   protected:
    void update(LEDCurve* led_curve);
    void update_heat();

   private:
    uint8_t* heat;
    uint8_t resolution;
    uint8_t* projection;
    uint8_t cooling;
    uint8_t sparking;
};
}  // namespace LEDGeometry
