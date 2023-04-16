#ifndef FRAGMENTSHADER_H
#define FRAGMENTSHADER_H

#include "Fragment.h"
#include "Shader.h"

template <class T1>
class FragmentShader : public Shader
{
private:
    ShaderType Type()
    {
        return Fragment_Shader;
    }

    void Input(void* data)
    {
        Fragment frag = *((Fragment*)data);
        
        void* data0 = VertexOut(frag.InAttrib0);

        T1 In0 = *((T1*)data0);
        void* data1 = VertexOut(frag.InAttrib1);
        T1 In1 = *((T1*)data1);
        void* data2 = VertexOut(frag.InAttrib2);
        T1 In2 = *((T1*)data2);

        In = (In0 * frag.UVW.s + In1 * frag.UVW.t + In2 * frag.UVW.p) * (1/frag.W);
    }

protected:
    T1 In;
};

#endif