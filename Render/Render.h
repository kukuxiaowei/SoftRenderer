#ifndef RENDER_H
#define RENDER_H

#include "Fragment.h"
#include "Shader.h"
#include "ShaderProgram.h"
#include "Buffer.h"
#include "VertexArray.h"
#include "Texture.h"

struct Viewport
{
    int Left;
    int Buttom;
    size_t Width;
    size_t Height;
};

enum BindBufferType
{
    ArrayBuffer,
    ElementArrayBuffer,
};

enum BindTextureType
{
    Texture2D,
};

enum RenderMode
{
    Point,
    Line,
    Fill,
};

class Render
{
private:
    RenderMode m_RenderMode = Fill;
    
    std::vector<Fragment> m_FragmentGroup;
    Viewport m_Viewport;

    std::vector<std::shared_ptr<Shader>> m_Shaders;
    std::vector<std::shared_ptr<ShaderProgram>> m_ShaderPrograms;
    unsigned int m_CurProgram;

    std::vector<std::shared_ptr<Buffer>> m_Buffers;
    unsigned int m_ArrayBuffer;//VBO
    unsigned int m_ElementArrayBuffer;//EBO
    std::vector<std::shared_ptr<VertexArray>> m_VertexArrays;
    unsigned int m_CurVertexArray;//VAO

    std::vector<std::shared_ptr<Texture>> m_Textures;
    unsigned int m_Texture2D;
    int m_ActiveTextures[16];//16个纹理单元

    vec4 m_ClearColor;
public:
    std::shared_ptr<Buffer> Context;

    Render();

    void Viewport(int x, int y, size_t width, size_t height);

    void ClearColor(float r, float g, float b, float a);
    void Clear();

    unsigned int CreateShader(std::shared_ptr<Shader> shaderSource);
    void DeleteShader(unsigned int shader);
    unsigned int CreateProgram();
    void AttachShader(unsigned int program, unsigned int shader);
    void LinkProgram(unsigned int program);
    void UseProgram(unsigned int program);
    void DeleteProgram(unsigned int program);

    void GenBuffers(size_t size, unsigned int* buffers);
    void BindBuffer(BindBufferType target, unsigned int buffer);
    void BufferData(BindBufferType target, size_t size, const void* data);
    void DeleteBuffers(size_t size, unsigned int* buffers);

    void GenVertexArrays(size_t size, unsigned int* arrays);
    void BindVertexArray(unsigned int array);
    void VertexAttribPointer(unsigned int index, int size, Type type, size_t stride, const int offset);
    void DeleteVertexArrays(size_t size, unsigned int* arrays);

    void GenTextures(size_t size, unsigned int* textures);
    void BindTexture(BindTextureType target, unsigned int texture);
    void TexImage2D(BindTextureType target, int level, TextureFormat internalFormat, size_t width, size_t height, TextureFormat format, Type type, void* data);
    void ActiveTexture(unsigned int unit);

    int GetUniformLocation(unsigned int program, const char* name);
    void UniformMatrix4fv(int location, const mat4* value);
    void Uniform4f(int location, float f0, float f1, float f2, float f3);
    void Uniform3f(int location, float f0, float f1, float f2);
    void Uniform1i(int location, int i0);
    
    template <typename T>
    void Uniform(int location, T value)
    {
        void* p = malloc(sizeof(T));
        T* data = (T*)p;
        *data = value;
        m_ShaderPrograms[m_CurProgram]->SetUniform(location, p);
    }

    void DrawArrays(int first, size_t count);
    void DrawElements(size_t count, Type type, const int offset);

    void PolygonMode(RenderMode mode);

    std::shared_ptr<Texture> GetTexture(Sampler2D sampler);
private:
    template <typename T>
    unsigned int Insert(T element, std::vector<T>& container);

    void DrawTriangle(int indices[3]);
    vec4 ViewportTransform(const vec4& in);
    void Rasterization(int indices[3], vec4 points[3]);
    bool IsDraw(vec2& A, vec2& B, vec2& C, vec2& P);
    bool IsInsideTriangle(vec2& A, vec2& B, vec2& C, vec2& P);
    bool IsInTriangleEdge(vec2& A, vec2& B, vec2& C, vec2& P);
    bool IsInTrianglePoint(vec2& A, vec2& B, vec2& C, vec2& P);
    vec3 Barycentric(vec2& A, vec2& B, vec2& C, vec2& P);
    void FillTriangle();

    void ResetState();

    unsigned int m_CurDrawCallId = 0;//TODO 删除
};

extern std::unique_ptr<Render> Renderer;

#endif