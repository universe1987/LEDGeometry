#pragma once
#include <stdint.h>

namespace LEDGeometry {
class Shape {
   public:
    Shape(int num_points) : num_points(num_points){};
    virtual ~Shape() {}
    // x coordinate
    virtual float x(int i) const = 0;
    // y coordinate
    virtual float y(int i) const = 0;
    // Euclidean distance to the origin
    float r(int i) const;
    // number of points on the shape
    int n_points() const;
    // dot product of the i-th point with (px, py)
    float projection(int i, float px, float py) const;
    // distance between the i-th point and (px, py)
    float distance(int i, float px, float py) const;
    // min and max dot product with (px, py)
    float min_projection(float px, float py) const;
    float max_projection(float px, float py) const;
    // min and max distance to (px, py)
    float min_distance(float px, float py) const;
    float max_distance(float px, float py) const;

   private:
    int num_points;
};
}  // namespace LEDGeometry
