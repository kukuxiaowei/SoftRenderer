#ifndef SHADERTest_H
#define SHADERTest_H

#include "VertexShader.h"
#include "FragmentShader.h"

struct InputTest
{
    vec3 aPos;
    vec3 aNormal;
    vec2 aTexCoords;
};

struct OutputTest
{
    vec3 FragPos;
    vec3 Normal;
    vec2 TexCoords;

    OutputTest operator+(const OutputTest& v)
    {
        OutputTest ret;
        ret.FragPos = this->FragPos + v.FragPos;
        ret.Normal = this->Normal + v.Normal;
        ret.TexCoords = this->TexCoords + v.TexCoords;
        return ret;        
    }

    OutputTest operator*(float f)
    {
        OutputTest ret;
        ret.FragPos = this->FragPos * f;
        ret.Normal = this->Normal * f;
        ret.TexCoords = this->TexCoords * f;
        return ret;
    }
};

class VertexShaderTest : public VertexShader<InputTest, OutputTest>
{
public:
    vec4 Vert()
    {
        mat4 model = *((mat4*)Uniform("model"));
        mat4 view = *((mat4*)Uniform("view"));
        mat4 projection = *((mat4*)Uniform("projection"));

        Out.FragPos = vec3(model * vec4(In.aPos, 1.0));
        //Out.Normal = In.aNormal;
        Out.Normal = mat3(transpose(inverse(model))) * In.aNormal;
        Out.TexCoords = In.aTexCoords;
        return projection * view * model * vec4(In.aPos, 1.0);
    }
};

class FragmentShaderTest : public FragmentShader<OutputTest>
{
public:
    vec4 Frag()
    {
        Sampler2D texture_diffuse1 = *((Sampler2D*)Uniform("texture_diffuse1"));
        Sampler2D texture_specular1 = *((Sampler2D*)Uniform("texture_specular1"));

        // ambient
        vec3 ambient = vec3(0.2f,0.2f,0.2f) * vec3(Texture(texture_diffuse1, In.TexCoords));

        // diffuse 
        vec3 norm = normalize(In.Normal);
        vec3 lightDir = normalize(vec3(1.2f, 1.0f, 2.0f) - In.FragPos);
        float diff = max(dot(norm, lightDir), 0.0f);
        vec3 diffuse = vec3(0.5f,0.5f,0.5f) * diff * vec3(Texture(texture_diffuse1, In.TexCoords));

        vec3 viewPos = *((vec3*)Uniform("viewPos"));

        // specular
        vec3 viewDir = normalize(viewPos - In.FragPos);
        vec3 reflectDir = reflect(-lightDir, norm);  
        float spec = pow(max(dot(viewDir, reflectDir), 0.0f), 32.0f);
        vec3 specular = vec3(1,1,1) * spec * vec3(Texture(texture_specular1, In.TexCoords));

        vec3 result = ambient + diffuse + specular;
        vec4 FragColor = vec4(result, 1.0);
        return FragColor;
    }
};

#endif