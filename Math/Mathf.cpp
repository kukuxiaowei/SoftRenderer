#include "Mathf.h"

float cross(const vec2& v0, const vec2& v1)
{
    return v0.x * v1.y - v0.y * v1.x;
}