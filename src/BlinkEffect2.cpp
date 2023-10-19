#include "BlinkEffect2.h"

#include "ColorScheduler.h"
#include "LEDCurve.h"
#include "Shape.h"

namespace LEDGeometry {

BlinkEffect2::BlinkEffect2()
    : left_eye_start(70),
      left_eye_end(74),
      right_eye_start(77),
      right_eye_end(81),
      mouth_start(91),
      mouth_end(100),
      left_eye_center(-2284),
      right_eye_center(2054),
      mouth_center(2593),
      left_eye_radius(1305),
      right_eye_radius(1262),
      mouth_radius(1513),
      counter(0),
      state(0) {}

void BlinkEffect2::update(LEDCurve* led_curve) {
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
                float d = led_curve->shape->x(i) - center;
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
                float d = led_curve->shape->x(i) - center;
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
