#ifndef TEXTURE_H
#define TEXTURE_H

#include "Mathf.h"
#include "Type.h"

typedef int Sampler2D;

enum TextureFormat
{
    RED,
    RGB,
    RGBA,
};

class Texture
{
private:
    int m_Level;
    size_t m_Width;
    size_t m_Height;
    TextureFormat m_Format;
    const void* m_Data;
    TextureFormat m_DataFormat;
public:
    void TexImage2D(int level, TextureFormat internalFormat, size_t width, size_t height, TextureFormat format, Type type, void* data)
    {
        m_Level = level;
        m_Width = width;
        m_Height = height;
        m_Format = internalFormat;
        m_Data = data;
        m_DataFormat = format;
    }

    vec4 Sample(vec2 texCoord)
    {
        //默认REPEAT:重复纹理图像
        while(texCoord.s < 0)
        {
            texCoord.s = texCoord.s + 1;
        }
        while(texCoord.t < 0)
        {
            texCoord.t = texCoord.t + 1;
        }

        //默认LINEAR:线性过滤
        vec2 sample = texCoord * vec2(m_Width, m_Height);
        unsigned int x1 = sample.x;
        unsigned int y1 = sample.y;
        unsigned int x2 = x1 + 1;
        unsigned int y2 = y1 + 1;

        float w11 = ((float)x2 - sample.x) * ((float)y2 - sample.y);
        float w21 = (sample.x - (float)x1) * ((float)y2 - sample.y);
        float w12 = ((float)x2 - sample.x) * (sample.y - (float)y1);
        float w22 = (sample.x - (float)x1) * (sample.y - (float)y1);

        vec4 color11;
        vec4 color12;
        vec4 color21;
        vec4 color22;
        if(m_Format == RGB && m_DataFormat == RGB)
        {
            color11.a = color12.a = color21.a = color22.a = 1;
            
            unsigned char* p;
            p = (unsigned char*)m_Data;
            p = p + (x1 + y1 * m_Width) * 3;
            color11.r = (float)p[0]/255.0f;
            color11.g = (float)p[1]/255.0f;
            color11.b = (float)p[2]/255.0f;

            p = (unsigned char*)m_Data;
            p = p + (x2 + y1 * m_Width) * 3;
            color21.r = (float)p[0]/255.0f;
            color21.g = (float)p[1]/255.0f;
            color21.b = (float)p[2]/255.0f;

            p = (unsigned char*)m_Data;
            p = p + (x1 + y2 * m_Width) * 3;
            color12.r = (float)p[0]/255.0f;
            color12.g = (float)p[1]/255.0f;
            color12.b = (float)p[2]/255.0f;

            p = (unsigned char*)m_Data;
            p = p + (x2 + y2 * m_Width) * 3;
            color22.r = (float)p[0]/255.0f;
            color22.g = (float)p[1]/255.0f;
            color22.b = (float)p[2]/255.0f;
        }
        else if(m_Format == RGBA && m_DataFormat == RGBA)
        {
            unsigned char* p;
            p = (unsigned char*)m_Data;
            p = p + (x1 + y1 * m_Width) * 4;
            color11.r = (float)p[0]/255.0f;
            color11.g = (float)p[1]/255.0f;
            color11.b = (float)p[2]/255.0f;
            color11.a = (float)p[3]/255.0f;

            p = (unsigned char*)m_Data;
            p = p + (x2 + y1 * m_Width) * 4;
            color21.r = (float)p[0]/255.0f;
            color21.g = (float)p[1]/255.0f;
            color21.b = (float)p[2]/255.0f;
            color21.a = (float)p[3]/255.0f;

            p = (unsigned char*)m_Data;
            p = p + (x1 + y2 * m_Width) * 4;
            color12.r = (float)p[0]/255.0f;
            color12.g = (float)p[1]/255.0f;
            color12.b = (float)p[2]/255.0f;
            color12.a = (float)p[3]/255.0f;

            p = (unsigned char*)m_Data;
            p = p + (x2 + y2 * m_Width) * 4;
            color22.r = (float)p[0]/255.0f;
            color22.g = (float)p[1]/255.0f;
            color22.b = (float)p[2]/255.0f;
            color22.a = (float)p[3]/255.0f;
        }

        vec4 color = color11 * w11 + color21 * w21 + color12 * w12 + color22 * w22;
        return color;
    }
};

#endif