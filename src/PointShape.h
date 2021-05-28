#include <stdint.h>

#include "Shape.h"

namespace LEDGeometry {
// Use PointShape when you do not care about the position of the points.
class PointShape : public Shape {
   public:
    PointShape(uint8_t n_points) : Shape(n_points) {}
    float x(uint8_t i) const { return 0; }
    float y(uint8_t i) const { return 0; }
};
}  // namespace LEDGeometry
