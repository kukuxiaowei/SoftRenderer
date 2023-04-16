#ifndef MATHF_H
#define MATHF_H

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

using namespace glm;

float cross(const vec2& v0, const vec2& v1);

template<typename T>
struct Rect
{
    T Xmin;
    T Xmax;
    T Ymin;
    T Ymax;
};

#endif