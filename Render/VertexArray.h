#ifndef VERTEXARRAY_H
#define VERTEXARRAY_H

#include <vector>
#include "Type.h"

struct VertexAttribPointer
{
    unsigned int Index;
    int Size;
    Type type;
    size_t Stride;
    const void* Pointer;
};

class VertexArray
{
private:
    std::vector<VertexAttribPointer> m_Pointers;
    int m_VertexSize = 0;

    void SetSize(int size, Type type)
    {
        switch (type)
        {
        case Float_Type:
            m_VertexSize += sizeof(float) * size;
            break;
        
        default:
            break;
        }
    }
public:
    void AddVertexAttribPointer(unsigned int index, int size, Type type, size_t stride, const void* pointer)
    {
        VertexAttribPointer p = {index, size, type, stride, pointer};
        m_Pointers.push_back(p);

        SetSize(size, type);
    };

    void GetVertex(int index, int* size, unsigned char** data)
    {
        //临时做法 总是取m_Pointers[0]的data。
        *size = m_VertexSize;
        
        VertexAttribPointer p = m_Pointers[0];
        *data = (unsigned char*)p.Pointer + p.Stride * index;
    }
};

#endif