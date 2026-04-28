#include "BlinkEffect.h"

#include "ColorScheduler.h"
#include "LEDCurve.h"
#include "Shape.h"
#include "Type.h"

namespace LEDGeometry {

BlinkEffect::BlinkEffect()
    : left_eye_start(78),
      left_eye_end(95),
      right_eye_start(107),
      right_eye_end(119),
      mouth_start(0),
      mouth_end(73),
      left_eye_center(-5204),
      right_eye_center(-5069),
      mouth_center(54),
      left_eye_radius(2228),
      right_eye_radius(2034),
      mouth_radius(9153),
      counter(0),
      state(0) {}

void BlinkEffect::update(LEDCurve* led_curve) {
    CRGB color = led_curve->color_scheduler->next_color();
    counter += 4;
    if (state == 0) {
        float center = (float)left_eye_center / 10000;
        for (int i = 0; i < led_curve->shape->n_points(); i++) {
            if (i < left_eye_start || i > left_eye_end) {
                led_curve->leds[i] = color;
            } else {
                float pct = 1 - ((int)counter / 100);
                if (pct < 0) {
                    pct = -pct;
                }
                float threshold = 0.05 + pct * (float)left_eye_radius / 10000;
                float d = led_curve->shape->y(i) - center;
                if (d <= threshold) {
                    led_curve->leds[i] = color;
                } else {
                    led_curve->leds[i] = CRGB::Black;
                }
            }
        }
    } else if (state == 1) {
        // right eye
        float center = (float)right_eye_center / 10000;
        for (int i = 0; i < led_curve->shape->n_points(); i++) {
            if (i < right_eye_start || i > right_eye_end) {
                led_curve->leds[i] = color;
            } else {
                float pct = 1 - ((int)counter / 100);
                if (pct < 0) {
                    pct = -pct;
                }
                float threshold = 0.05 + pct * (float)right_eye_radius / 10000;
                float d = led_curve->shape->y(i) - center;
                if (d <= threshold) {
                    led_curve->leds[i] = color;
                } else {
                    led_curve->leds[i] = CRGB::Black;
                }
            }
        }
    } else {
        float center = (float)mouth_center / 10000;
        for (int i = 0; i < led_curve->shape->n_points(); i++) {
            if (i < mouth_start || i > mouth_end) {
                led_curve->leds[i] = color;
            } else {
                float pct = 1 - ((int)counter / 100);
                if (pct < 0) {
                    pct = -pct;
                }
                float threshold = 0.1 + pct * (float)mouth_radius / 10000;
                float d = led_curve->shape->y(i) - center;
                if (d <= threshold) {
                    led_curve->leds[i] = color;
                } else {
                    led_curve->leds[i] = CRGB::Black;
                }
            }
        }
    }
    if (counter > 200) {
        counter = 0;
        state++;
        if (state > 2) {
            state = 0;
        }
    }
}
}  // namespace LEDGeometry
