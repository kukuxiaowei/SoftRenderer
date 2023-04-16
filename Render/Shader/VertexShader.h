#ifndef VERTEXSHADER_H
#define VERTEXSHADER_H

#include "Shader.h"

template <class T1, class T2>
class VertexShader : public Shader
{
private:
    ShaderType Type()
    {
        return Vertex_Shader;
    }

    void Input(void* data)
    {
        In = *((T1*)data);
    }

    void Output(int* size, void** data)
    {
        *size = sizeof(T2);
        *data = &Out;
    }

protected:
    T1 In;
    T2 Out;
};

#endif