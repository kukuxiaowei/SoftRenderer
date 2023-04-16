#ifndef SHADERPROGRAM_H
#define SHADERPROGRAM_H

#include <vector>
#include <unordered_map>
#include <memory>
#include "Shader.h"

class ShaderProgram
{
private:
    std::vector<std::shared_ptr<Shader>> m_Shaders;

    //临时做法 把顶点着色器输出暂存（片元使用）
    std::vector<void*> m_VertexOutData;

    std::unordered_map<std::string, int> m_UniformMap;
    std::vector<void*> m_UniformSource;

    int CreateUniform(std::string name)
    {
        int location = m_UniformSource.size();
        m_UniformSource.push_back(nullptr);
        m_UniformMap.insert(std::make_pair(name, location));
        return location;
    }

    void SetData(std::vector<void*>& dataGroup, int index, void* data)
    {
        void* data_old = dataGroup[index];
        dataGroup[index] = data;
        if(data_old != nullptr)
        {
            free(data_old);
        }
    }

    void ReleaseData(std::vector<void*>& dataGroup)
    {
        int count = dataGroup.size();
        for(int i = 0; i < count; i++)
        {
            void* data_old = dataGroup[i];
            if(data_old != nullptr)
            {
                free(data_old);
            }
        }
    }

public:
    ~ShaderProgram()
    {
        ReleaseData(m_VertexOutData);
        ReleaseData(m_UniformSource);
    }

    int GetUniformLocation(std::string name)
    {
        int location;
        std::unordered_map<std::string, int>::iterator it;
        if ((it = m_UniformMap.find(name)) != m_UniformMap.end())
        {
            location = it->second;
        }
        else
        {
            location = CreateUniform(name);
        }
        return location;
    }

    void SetUniform(int location, void* data)
    {
        SetData(m_UniformSource, location, data);
    }

    void AttachShader(std::shared_ptr<Shader> shader)
    {
        shader->Program = this;
        m_Shaders.push_back(shader);
    }

    void LinkProgram()
    {
        //TODO
    }

    std::shared_ptr<Shader> GetShader(ShaderType shaderType)
    {
        for(int i = 0; i < m_Shaders.size(); i++)
        {
            if(m_Shaders[i]->Type() == shaderType)
            {
                return m_Shaders[i];
            }
        }
        return nullptr;
    }

    void Uniform(std::string name, void** data)
    {
        std::unordered_map<std::string, int>::iterator it;
        if ((it = m_UniformMap.find(name)) != m_UniformMap.end())
        {
            *data = m_UniformSource[it->second];
            return;
        }
    }

    void SetVertexOutData(int vertexIndex, int size, void* data)
    {
        if(m_VertexOutData.size() < vertexIndex + 1)
        {
            m_VertexOutData.resize(vertexIndex + 1);
        }

        void* vertexOutData = malloc(size);
        memcpy(vertexOutData, data, size);
        m_VertexOutData[vertexIndex] = vertexOutData;
    }

    void GetVertexOutData(int vertexIndex, void** data)
    {
        *data = m_VertexOutData[vertexIndex];
    }
};

#endif