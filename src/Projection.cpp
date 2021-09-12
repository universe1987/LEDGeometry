#include "Projection.h"

#include <FastLED.h>
#include <math.h>

#include "Shape.h"

namespace LEDGeometry {

void radial_projection(Shape* shape, uint8_t* projection, uint8_t resolution,
                       float center_x, float center_y) {
    float min_dist = shape->min_distance(center_x, center_y);
    float max_dist = shape->max_distance(center_x, center_y);
    float factor = (resolution - 1) / (max_dist - min_dist);
    for (int i = 0; i < shape->n_points(); i++) {
        float dist = shape->distance(i, center_x, center_y);
        projection[i] = (uint8_t)round((dist - min_dist) * factor);
    }
}

void radial_projection(Shape* shape, uint8_t* projection, uint8_t resolution) {
    uint16_t theta = random16();
    float center_x = (float)cos16(theta) / 32767;
    float center_y = (float)sin16(theta) / 32767;
    radial_projection(shape, projection, resolution, center_x, center_y);
}

void parallel_projection(Shape* shape, uint8_t* projection, uint8_t resolution,
                         float direction_x, float direction_y) {
    float min_proj = shape->min_projection(direction_x, direction_y);
    float max_proj = shape->max_projection(direction_x, direction_y);
    float factor = (resolution - 1) / (max_proj - min_proj);
    for (int i = 0; i < shape->n_points(); i++) {
        float proj = shape->projection(i, direction_x, direction_y);
        projection[i] = (uint8_t)round((proj - min_proj) * factor);
    }
}

void parallel_projection(Shape* shape, uint8_t* projection,
                         uint8_t resolution) {
    uint16_t theta = random16();
    float direction_x = (float)cos16(theta) / 32767;
    float direction_y = (float)sin16(theta) / 32767;
    parallel_projection(shape, projection, resolution, direction_x,
                        direction_y);
}

void intrinsic_projection(Shape* shape, uint8_t* projection,
                          uint8_t resolution) {
    uint8_t half = shape->n_points() / 2;
    for (uint8_t i = 0; i < half; i++) {
        projection[i] = resolution - half + i;
        projection[2 * half - i - 1] = resolution - half - i;
    }
}
}  // namespace LEDGeometry
