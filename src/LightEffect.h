#pragma once

namespace LEDGeometry {
class LEDCurve;

// Interface that controls the color of an LED curve.
class LightEffect {
   public:
    virtual ~LightEffect() {}
    // Assign color to each point on the curve based on the internal states,
    // then update the internal states.
    virtual void update(LEDCurve* led_curve) = 0;
};
}  // namespace LEDGeometry
