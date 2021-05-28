#include "Shape.h"

#include <math.h>

namespace LEDGeometry {
int Shape::n_points() const { return num_points; }

float Shape::r(int i) const {
    float xx = x(i);
    float yy = y(i);
    return sqrt(xx * xx + yy * yy);
}

float Shape::projection(int i, float px, float py) const {
    return px * x(i) + py * y(i);
}

float Shape::distance(int i, float px, float py) const {
    float x_i = x(i);
    float y_i = y(i);
    return sqrt((x_i - px) * (x_i - px) + (y_i - py) * (y_i - py));
}

float Shape::min_projection(float px, float py) const {
    float proj_min = 100;
    for (int i = 0; i < num_points; i++) {
        float p = projection(i, px, py);
        if (p < proj_min) proj_min = p;
    }
    return proj_min;
}

float Shape::max_projection(float px, float py) const {
    float proj_max = -100;
    for (int i = 0; i < num_points; i++) {
        float p = projection(i, px, py);
        if (p > proj_max) proj_max = p;
    }
    return proj_max;
}

float Shape::min_distance(float px, float py) const {
    float dist_min = 100;
    for (int i = 0; i < num_points; i++) {
        float d = distance(i, px, py);
        if (d < dist_min) dist_min = d;
    }
    return dist_min;
}

float Shape::max_distance(float px, float py) const {
    float dist_max = 0;
    for (int i = 0; i < num_points; i++) {
        float d = distance(i, px, py);
        if (d > dist_max) dist_max = d;
    }
    return dist_max;
}
}  // namespace LEDGeometry
