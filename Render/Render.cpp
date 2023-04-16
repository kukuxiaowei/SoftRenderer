#include "Render.h"

std::unique_ptr<Render> Renderer = std::make_unique<Render>();

Render::Render()
{
    Context = std::make_shared<Buffer>();
    this->ResetState();
}

void Render::Viewport(int x, int y, size_t width, size_t height)
{
    m_Viewport.Left = x;
    m_Viewport.Buttom = y;
    m_Viewport.Width = width;
    m_Viewport.Height = height;

    m_FragmentGroup.resize(width * height);
}

template <typename T>
unsigned int Render::Insert(T element, std::vector<T>& container)
{
    unsigned int id = 0;
    for(; id < container.size(); id++)
    {
        if(container[id] == nullptr)
        {
            break;
        }
    }

    if(id == container.size())
    {
        container.push_back(element);
    }
    else
    {
        container[id] = element;
    }

    return id;
}

unsigned int Render::CreateShader(std::shared_ptr<Shader> shaderSource)
{
    shaderSource->Render = this;
    return this->Insert(shaderSource, m_Shaders);
}

void Render::DeleteShader(unsigned int shader)
{
    m_Shaders[shader] = nullptr;
}

unsigned int Render::CreateProgram()
{
    std::shared_ptr<ShaderProgram> newProgram = std::make_shared<ShaderProgram>();
    return this->Insert(newProgram, m_ShaderPrograms);
}

void Render::AttachShader(unsigned int program, unsigned int shader)
{
    m_ShaderPrograms[program]->AttachShader(m_Shaders[shader]);
}

void Render::LinkProgram(unsigned int program)
{
    m_ShaderPrograms[program]->LinkProgram();
}

void Render::UseProgram(unsigned int program)
{
    m_CurProgram = program;
}

void Render::DeleteProgram(unsigned int program)
{
    m_ShaderPrograms[program] = nullptr;
}

void Render::GenBuffers(size_t size, unsigned int* buffers)
{
    for(unsigned int i = 0; i < size; i++)
    {
        std::shared_ptr<Buffer> newBuffer = std::make_shared<Buffer>();
        buffers[i] = this->Insert(newBuffer, m_Buffers);
    }
}

void Render::BindBuffer(BindBufferType target, unsigned int buffer)
{
    switch (target)
    {
    case ArrayBuffer:
        m_ArrayBuffer = buffer;
        break;
    case ElementArrayBuffer:
        m_ElementArrayBuffer = buffer;
        break;
    
    default:
        break;
    }
}

void Render::BufferData(BindBufferType target, size_t size, const void* data)
{
    switch (target)
    {
    case ArrayBuffer:
        m_Buffers[m_ArrayBuffer]->SetData(size, data);
        break;
    case ElementArrayBuffer:
        m_Buffers[m_ElementArrayBuffer]->SetData(size, data);
        break;
    
    default:
        break;
    }
}

void Render::DeleteBuffers(size_t size, unsigned int* buffers)
{
    for(unsigned int i = 0; i < size; i++)
    {
        int id = buffers[i];
        m_Buffers[id] = nullptr;
    }
}

void Render::GenVertexArrays(size_t size, unsigned int* arrays)
{
    for(unsigned int i = 0; i < size; i++)
    {
        std::shared_ptr<VertexArray> newArray = std::make_shared<VertexArray>();
        arrays[i] = this->Insert(newArray, m_VertexArrays);
    }
}

void Render::BindVertexArray(unsigned int array)
{
    m_CurVertexArray = array;
}

void Render::VertexAttribPointer(unsigned int index, int size, Type type, size_t stride, const int offset)
{
    size_t dataSize;
    const void* data;
    m_Buffers[m_ArrayBuffer]->GetData(&dataSize, &data);
    unsigned int* pointer = (unsigned int*)data + offset;
    m_VertexArrays[m_CurVertexArray]->AddVertexAttribPointer(index, size, type, stride, pointer);
}

void Render::DeleteVertexArrays(size_t size, unsigned int* arrays)
{
    for(unsigned int i = 0; i < size; i++)
    {
        int id = arrays[i];
        m_VertexArrays[id] = nullptr;
    }
}

void Render::GenTextures(size_t size, unsigned int* textures)
{
    for(unsigned int i = 0; i < size; i++)
    {
        std::shared_ptr<Texture> newTexture = std::make_shared<Texture>();
        textures[i] = this->Insert(newTexture, m_Textures);
    }
}

void Render::BindTexture(BindTextureType target, unsigned int texture)
{
    switch (target)
    {
    case Texture2D:
        m_Texture2D = texture;
        break;
    
    default:
        break;
    }
}

void Render::TexImage2D(BindTextureType target, int level, TextureFormat internalFormat, size_t width, size_t height, TextureFormat format, Type type, void* data)
{
    switch (target)
    {
    case Texture2D:
        m_Textures[m_Texture2D]->TexImage2D(level, internalFormat, width, height, format, type, data);
        break;
    
    default:
        break;
    }
}

void Render::ActiveTexture(unsigned int unit)
{
    if(unit >= 16) return;
    m_ActiveTextures[unit] = m_Texture2D;
}

int Render::GetUniformLocation(unsigned int program, const char* name)
{
    return m_ShaderPrograms[program]->GetUniformLocation(name);
}

void Render::UniformMatrix4fv(int location, const mat4* value)
{
    void* p = malloc(sizeof(mat4));
    mat4* data = (mat4*)p;
    *data = *value;
    m_ShaderPrograms[m_CurProgram]->SetUniform(location, p);
}

void Render::Uniform4f(int location, float f0, float f1, float f2, float f3)
{
    void* p = malloc(sizeof(float) * 4);
    float* data = (float*)p;
    data[0] = f0;
    data[1] = f1;
    data[2] = f2;
    data[3] = f3;
    m_ShaderPrograms[m_CurProgram]->SetUniform(location, p);
}

void Render::Uniform3f(int location, float f0, float f1, float f2)
{
    void* p = malloc(sizeof(float) * 3);
    float* data = (float*)p;
    data[0] = f0;
    data[1] = f1;
    data[2] = f2;
    m_ShaderPrograms[m_CurProgram]->SetUniform(location, p);
}

void Render::Uniform1i(int location, int i0)
{
    void* p = malloc(sizeof(int));
    int* data = (int*)p;
    *data = i0;
    m_ShaderPrograms[m_CurProgram]->SetUniform(location, p);
}

void Render::DrawArrays(int first, size_t count)
{
    m_CurDrawCallId++;

    for(int i = 0; i < count; i += 3)
    {
        int indices[3] = {i+first, i+1+first, i+2+first};
        this->DrawTriangle(indices);
    }
    
    this->FillTriangle();
}

void Render::DrawElements(size_t count, Type type, const int offset)
{
    m_CurDrawCallId++;

    size_t dataSize;
    const void* data;
    m_Buffers[m_ElementArrayBuffer]->GetData(&dataSize, &data);
    int* pointer = (int*)data + offset;

    for(int i = 0; i < count; i += 3)
    {
        //if(type == Int_Type)
        {
            int indices[3] = {pointer[i], pointer[i+1], pointer[i+2]};
            this->DrawTriangle(indices);
        }

    }

    this->FillTriangle();
}

void Render::DrawTriangle(int indices[3])
{
    std::shared_ptr<VertexArray> vertexArray = m_VertexArrays[m_CurVertexArray];
    std::shared_ptr<Shader> vertexShader = m_ShaderPrograms[m_CurProgram]->GetShader(Vertex_Shader);
    vec4 points[3];
    for(int i = 0; i < 3; i++)
    {
        int size;
        unsigned char* data;
        vertexArray->GetVertex(indices[i], &size, &data);
        vertexShader->Input(data);
        vec4 out = vertexShader->Vert();
        //临时做法 避免除零
        if(out.w < 0.01f) return;
        out.x /= out.w;
        out.y /= out.w;
        out.z /= out.w;
        out.w = 1/out.w;//透视投影矫正

        int vertShaderOutSize;
        void* vertShaderOutData;
        vertexShader->Output(&vertShaderOutSize, &vertShaderOutData);
        m_ShaderPrograms[m_CurProgram]->SetVertexOutData(indices[i], vertShaderOutSize, vertShaderOutData);
        
        points[i] = this->ViewportTransform(out);
    }

    this->Rasterization(indices, points);
}

//视口变换
//NDC->Screen
vec4 Render::ViewportTransform(const vec4& in)
{
    vec4 out;
    out.x = (in.x + 1)/2 * m_Viewport.Width + m_Viewport.Left;
    out.y = (in.y + 1)/2 * m_Viewport.Height + m_Viewport.Buttom;
    out.z = (in.z + 1)/2;
    out.w = in.w;
    return out;
}

//光栅化插值
void Render::Rasterization(int indices[3], vec4 points[3])
{
    vec2 A = vec2(points[0].x, points[0].y);
    vec2 B = vec2(points[1].x, points[1].y);
    vec2 C = vec2(points[2].x, points[2].y);

    //三角形包围盒 补误差
    Rect<int> box;
    box.Xmin = min(min(A.x, B.x), C.x) - 1;
    box.Xmax = max(max(A.x, B.x), C.x) + 1;
    box.Ymin = min(min(A.y, B.y), C.y) - 1;
    box.Ymax = max(max(A.y, B.y), C.y) + 1;

    box.Xmin = clamp(box.Xmin, 0, (int)m_Viewport.Width - 1);
    box.Xmax = clamp(box.Xmax, 0, (int)m_Viewport.Width - 1);
    box.Ymin = clamp(box.Ymin, 0, (int)m_Viewport.Height - 1);
    box.Ymax = clamp(box.Ymax, 0, (int)m_Viewport.Height - 1);

    for(int x = box.Xmin; x < box.Xmax; x++)
    {
        for(int y = box.Ymin; y < box.Ymax; y++)
        {
            vec2 P = vec2(x, y);
            if(this->IsDraw(A, B, C, P))
            {
                vec3 UVW = this->Barycentric(A, B, C, P);

                int pos = x + (m_Viewport.Height - 1 - y) * m_Viewport.Width;
                
                //深度测试
                //Z-Interpolation
                float z = points[0].z * UVW.s + points[1].z * UVW.t + points[2].z * UVW.p;
                //Z-Test
                float z_old = m_FragmentGroup[pos].Z;
                if(z > z_old)
                {
                    continue;
                }

                //透视投影矫正
                //1/W-Interpolation
                float w = points[0].w * UVW.s + points[1].w * UVW.t + points[2].w * UVW.p;
                //Perspective-Correct Interpolation
                UVW.s *= points[0].w;
                UVW.t *= points[1].w;
                UVW.p *= points[2].w;

                m_FragmentGroup[pos].DrawCallId = m_CurDrawCallId;
                m_FragmentGroup[pos].InAttrib0 = indices[0];
                m_FragmentGroup[pos].InAttrib1 = indices[1];
                m_FragmentGroup[pos].InAttrib2 = indices[2];
                m_FragmentGroup[pos].Z = z;
                m_FragmentGroup[pos].W = w;
                m_FragmentGroup[pos].UVW = UVW;
            }
        }
    }
}

bool Render::IsDraw(vec2& A, vec2& B, vec2& C, vec2& P)
{
    switch (m_RenderMode)
    {
    case Fill:
        return this->IsInsideTriangle(A, B, C, P);
    case Line:
        return this->IsInTriangleEdge(A, B, C, P);
    case Point:
        return this->IsInTrianglePoint(A, B, C, P);
    default:
        return false;
    }
}

bool Render::IsInTrianglePoint(vec2& A, vec2& B, vec2& C, vec2& P)
{
    return (A == P || B == P || C == P);
}

bool Render::IsInTriangleEdge(vec2& A, vec2& B, vec2& C, vec2& P)
{
    float threshold = 0.005;
    vec2 UV = Barycentric(A, B, C, P);
    float u = UV.x;
    float v = UV.y;
    float w = 1 - u - v;
    if(-threshold < u && u < threshold) return true;
    if(-threshold < w && w < threshold) return true;
    if(-threshold < v && v < threshold) return true;
    return false;
}

bool Render::IsInsideTriangle(vec2& A, vec2& B, vec2& C, vec2& P)
{
    bool dir;
    vec2 AB = B - A;
    vec2 AP = P - A;
    dir = cross(AB, AP) <= 0.0f;
    vec2 BC = C - B;
    vec2 BP = P - B;
    if(dir != cross(BC, BP) <= 0.0f) return false;
    vec2 CA = A - C;
    vec2 CP = P - C;
    if(dir != cross(CA, CP) <= 0.0f) return false;
    return true;
}

vec3 Render::Barycentric(vec2& A, vec2& B, vec2& C, vec2& P)
{
    vec2 AB = B - A;
    vec2 AP = P - A;

    vec2 BC = C - B;
    vec2 BP = P - B;

    vec2 CA = A - C;
    vec2 CP = P - C;

    vec2 AC = C - A;
    float area = cross(AB, AC);
    float u = cross(BC, BP)/area;
    float v = cross(CA, CP)/area;
    return vec3(u, v, 1 - u - v);
}

void Render::FillTriangle()
{
    std::shared_ptr<Shader> fragmentShader = m_ShaderPrograms[m_CurProgram]->GetShader(Fragment_Shader);
    int pixelCount = m_FragmentGroup.size();
    for(int i = 0; i < pixelCount; i++)
    {
        if(m_FragmentGroup[i].DrawCallId != m_CurDrawCallId)
        {
            continue;
        }

        if(m_FragmentGroup[i].Z > 1.0f)
        {
            continue;
        }

        unsigned char* data = (unsigned char*)&m_FragmentGroup[i];
        fragmentShader->Input(data);
        vec4 out = fragmentShader->Frag();

        size_t size;
        const void* frame;
        Context->GetData(&size, &frame);
        unsigned char* p = (unsigned char*)frame;
        p[i*4] = clamp((int)(out.z * 255), 0, 255);
        p[i*4+1] = clamp((int)(out.y * 255), 0, 255);
        p[i*4+2] = clamp((int)(out.x * 255), 0, 255);
        p[i*4+3] = clamp((int)(out.w * 255), 0, 255);
    }
}

void Render::PolygonMode(RenderMode mode)
{
    m_RenderMode = mode;
}

std::shared_ptr<Texture> Render::GetTexture(Sampler2D sampler)
{
    int acitveTex = m_ActiveTextures[sampler];
    if(acitveTex != -1)
    {
        return m_Textures[acitveTex];
    }
    return nullptr;
}

void Render::ClearColor(float r, float g, float b, float a)
{
    m_ClearColor = vec4(r, g, b, a);
}

void Render::Clear()
{
    size_t size;
    const void* data;
    Context->GetData(&size, &data);
    unsigned char* p = (unsigned char*)data;
    for(unsigned int i = 0; i < size; i += 4)
    {
        p[i+0] = m_ClearColor.b * 255;
        p[i+1] = m_ClearColor.g * 255;
        p[i+2] = m_ClearColor.r * 255;
        p[i+3] = m_ClearColor.a * 255;
    }

    this->ResetState();
}

void Render::ResetState()
{
    m_ClearColor = vec4();

    for(int i = 0; i < 16; i++)
    {
        m_ActiveTextures[i] = -1;
    }

    int pixelCount = m_FragmentGroup.size();
    for(int i = 0; i < pixelCount; i++)
    {
        m_FragmentGroup[i].DrawCallId = 1024;
        m_FragmentGroup[i].Z = 2.0f;
    }

    m_CurDrawCallId = 0;
}