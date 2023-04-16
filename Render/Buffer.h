#ifndef BUFFER_H
#define BUFFER_H

class Buffer
{
private:
    size_t m_Size;
    const void* m_Data;
public:
    void SetData(size_t size, const void* data)
    {
        m_Size = size;
        m_Data = data;
    }

    void GetData(size_t* size, const void** data)
    {
        *size = m_Size;
        *data = m_Data;
    }
};


#endif