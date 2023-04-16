#ifndef SHADER_H
#define SHADER_H

#include "Mathf.h"
#include "Texture.h"

class Render;
class ShaderProgram;

enum ShaderType
{
    Invalid,
    Vertex_Shader,
    Fragment_Shader
};

class Shader
{
public:
    Render* Render;
    ShaderProgram* Program;

    virtual ShaderType Type();

    virtual void Input(void* data);
    virtual void Output(int* size, void** data);

    virtual vec4 Vert();
    virtual vec4 Frag();

protected:
    void* Uniform(const char* name);
    vec4 Texture(Sampler2D sampler, vec2 texCoord);

    void* VertexOut(int index);//临时做法
};

#endif