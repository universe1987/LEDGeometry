#pragma once
#include <stdint.h>

#include "LightEffect.h"

namespace LEDGeometry {
class LEDCurve;

class BlinkEffect : public LightEffect {
   public:
    BlinkEffect();

   protected:
    void update(LEDCurve* led_curve);

   private:
    uint8_t left_eye_start;
    uint8_t left_eye_end;
    uint8_t right_eye_start;
    uint8_t right_eye_end;
    uint8_t mouth_start;
    uint8_t mouth_end;
    int16_t left_eye_center;
    int16_t right_eye_center;
    int16_t mouth_center;
    int16_t left_eye_radius;
    int16_t right_eye_radius;
    int16_t mouth_radius;
    uint8_t counter;
    uint8_t state;
};
}  // namespace LEDGeometry
