#include "Shader.h"
#include "ShaderProgram.h"
#include "Render.h"

ShaderType Shader::Type()
{
    return Invalid;
}

void Shader::Input(void* data)
{

}

void Shader::Output(int* size, void** data)
{

}

vec4 Shader::Vert()
{
    return vec4();
}

vec4 Shader::Frag()
{
    return vec4();
}

void* Shader::Uniform(const char* name)
{
    void* data;
    Program->Uniform(name, &data);
    return data;
}

vec4 Shader::Texture(Sampler2D sampler, vec2 texCoord)
{
    auto texture = Render->GetTexture(sampler);
    if(texture != nullptr)
    {
        return texture->Sample(texCoord);
    }
    return vec4(1,1,1,1);
}

void* Shader::VertexOut(int index)
{
    void* data;
    Program->GetVertexOutData(index, &data);
    return data;
}