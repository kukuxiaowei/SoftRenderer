#ifndef FRAGMENT_H
#define FRAGMENT_H

#include "Mathf.h"

struct Fragment
{
    unsigned int DrawCallId = 1024;//TODO 删除

    int InAttrib0;
    int InAttrib1;
    int InAttrib2;
    float Z = 2.0f;
    float W;
    vec3 UVW;
};

#endif