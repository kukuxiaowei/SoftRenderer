#ifndef MESH_H
#define MESH_H

#include "Render.h"

#include <string>
#include <vector>

struct Vertex
{
    vec3 Position;
    vec3 Normal;
    vec2 TexCoords;
    //vec3 Tangent;
    //vec3 Bitangent;
};

struct MeshTexture
{
    unsigned int id;
    std::string type;
    std::string path;
    unsigned char* data;
};

class Mesh
{
public:
    std::vector<Vertex>       Vertices;
    std::vector<unsigned int> Indices;
    std::vector<MeshTexture>  Textures;
    unsigned int VAO;

    Mesh(std::vector<Vertex> vertices, std::vector<unsigned int> indices, std::vector<MeshTexture> textures)
    {
        this->Vertices = vertices;
        this->Indices = indices;
        this->Textures = textures;

        SetupMesh();
    }

    void Draw(int shader) 
    {
        unsigned int diffuseNr  = 1;
        unsigned int specularNr = 1;
        unsigned int normalNr   = 1;
        unsigned int heightNr   = 1;
        for(unsigned int i = 0; i < Textures.size(); i++)
        {
            Renderer->BindTexture(Texture2D, Textures[i].id);//TODO 先Active后Bind
            Renderer->ActiveTexture(i);

            std::string number;
            std::string name = Textures[i].type;
            if(name == "texture_diffuse")
                number = std::to_string(diffuseNr++);
            else if(name == "texture_specular")
                number = std::to_string(specularNr++);
            else if(name == "texture_normal")
                number = std::to_string(normalNr++);
             else if(name == "texture_height")
                number = std::to_string(heightNr++);

            Renderer->Uniform1i(Renderer->GetUniformLocation(shader, (name + number).c_str()), i);
        }
        
        Renderer->BindVertexArray(VAO);
        Renderer->BindBuffer(ElementArrayBuffer, EBO);//TODO 删除
        Renderer->DrawElements(Indices.size(), Int_Type, 0);
        Renderer->BindVertexArray(0);

        Renderer->ActiveTexture(0);
    }

private:
    unsigned int VBO, EBO;

    void SetupMesh()
    {
        Renderer->GenVertexArrays(1, &VAO);
        Renderer->GenBuffers(1, &VBO);
        Renderer->GenBuffers(1, &EBO);

        Renderer->BindVertexArray(VAO);

        Renderer->BindBuffer(ArrayBuffer, VBO);
        Renderer->BufferData(ArrayBuffer, Vertices.size() * sizeof(Vertex), &Vertices[0]);  

        Renderer->BindBuffer(ElementArrayBuffer, EBO);
        Renderer->BufferData(ElementArrayBuffer, Indices.size() * sizeof(unsigned int), &Indices[0]);

        Renderer->VertexAttribPointer(0, 3, Float_Type, sizeof(Vertex), 0);
        Renderer->VertexAttribPointer(1, 3, Float_Type, sizeof(Vertex), offsetof(Vertex, Normal));
        Renderer->VertexAttribPointer(2, 2, Float_Type, sizeof(Vertex), offsetof(Vertex, TexCoords));
        //Renderer->VertexAttribPointer(3, 3, Float_Type, sizeof(Vertex), offsetof(Vertex, Tangent));
        //Renderer->VertexAttribPointer(4, 3, Float_Type, sizeof(Vertex), offsetof(Vertex, Bitangent));

        Renderer->BindVertexArray(0);
    }
};
#endif