#pragma once

#include <stdint.h>

namespace LEDGeometry {
class Shape;
// Projection is proportional to the distance to the center.
void radial_projection(Shape* shape, uint8_t* projection, uint8_t resolution,
                       float center_x, float center_y);
// Project with respect to a random center
void radial_projection(Shape* shape, uint8_t* projection, uint8_t resolution);

void parallel_projection(Shape* shape, uint8_t* projection, uint8_t resolution,
                         float direction_x, float direction_y);

void parallel_projection(Shape* shape, uint8_t* projection, uint8_t resolution);

void intrinsic_projection(Shape* shape, uint8_t* projection,
                          uint8_t resolution);
}  // namespace LEDGeometry
