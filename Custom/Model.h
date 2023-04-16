#ifndef MODEL_H
#define MODEL_H

#define STB_IMAGE_IMPLEMENTATION
#include <stb_image.h>

#define TINYOBJLOADER_IMPLEMENTATION
#include <tiny_obj_loader.h>

#include "Mesh.h"

#include <string>
#include <vector>

unsigned int TextureFromFile(const char *path, const std::string &directory, unsigned char** textureData, bool gamma = false);

class Model
{
public:
    std::vector<MeshTexture> TexturesLoaded;
    std::vector<Mesh> Meshes;
    std::string Directory;
    bool GammaCorrection;

    Model(std::string const &path, bool gamma = false) : GammaCorrection(gamma)
    {
        LoadModel(path);
    }

    ~Model()
    {
        for (const auto& texture : TexturesLoaded) 
        {
            if(texture.data)
            {
                stbi_image_free(texture.data);
            }
        }
    }

    void Draw(int shader)
    {
        for(unsigned int i = 0; i < Meshes.size(); i++)
            Meshes[i].Draw(shader);
    }

private:
    void LoadModel(std::string const &path)
    {
        this->Directory = path.substr(0, path.find_last_of('/'));
        
        tinyobj::attrib_t attrib;
        std::vector<tinyobj::shape_t> shapes;
	    std::vector<tinyobj::material_t> materials;
        std::string warn;
        std::string err;
        if (!tinyobj::LoadObj(&attrib, &shapes, &materials, &warn, &err, path.c_str(), this->Directory.c_str())) 
        {
		    //throw std::runtime_error(err);
	    }

        for (const auto& shape : shapes) 
        {
            Meshes.push_back(ProcessMesh(shape, attrib, materials));
        }
    }

    Mesh ProcessMesh(const tinyobj::shape_t& shape, const tinyobj::attrib_t& attrib, const std::vector<tinyobj::material_t>& materials)
    {
        std::vector<Vertex> vertices;
        std::vector<unsigned int> indices;
        std::vector<MeshTexture> textures;

        size_t index_offset = 0;
        for (size_t f = 0; f < shape.mesh.num_face_vertices.size(); f++)
        {
            int fv = shape.mesh.num_face_vertices[f];

            for (size_t v = 0; v < fv; v++)
            {
                tinyobj::index_t index = shape.mesh.indices[index_offset + v];

                Vertex vertex;
                vertex.Position.x = attrib.vertices[3 * index.vertex_index + 0];
                vertex.Position.y = attrib.vertices[3 * index.vertex_index + 1];
                vertex.Position.z = attrib.vertices[3 * index.vertex_index + 2];

                if (attrib.normals.size())
                {
                    vertex.Normal.x = attrib.normals[3 * index.normal_index + 0];
                    vertex.Normal.y = attrib.normals[3 * index.normal_index + 1];
                    vertex.Normal.z = attrib.normals[3 * index.normal_index + 2];
                }

                if (attrib.texcoords.size())
                {
                    vertex.TexCoords.x = attrib.texcoords[2 * index.texcoord_index + 0];
                    vertex.TexCoords.y = 1.0f - attrib.texcoords[2 * index.texcoord_index + 1];
                }

                vertices.push_back(vertex);
                indices.push_back(vertices.size()-1);
            }
            index_offset += fv;
        }

        //默认一个mesh只有一个材质
        int materialID = shape.mesh.material_ids[0];
        if(0 <= materialID && materialID < materials.size())
        {
            tinyobj::material_t material = materials[materialID];

            //diffuse
            if(!material.diffuse_texname.empty())
            {
                MeshTexture diffuse = LoadMaterialTextures(material.diffuse_texname, "texture_diffuse");
                textures.push_back(diffuse);
            }
            //specular
            if(!material.specular_texname.empty())
            {
                MeshTexture specular = LoadMaterialTextures(material.specular_texname, "texture_specular");
                textures.push_back(specular);
            }
            //normal
            if(!material.normal_texname.empty())
            {
                MeshTexture normal = LoadMaterialTextures(material.normal_texname, "texture_normal");
                textures.push_back(normal);
            }
        }

        return Mesh(vertices, indices, textures);
    }

    MeshTexture LoadMaterialTextures(std::string texPath, std::string typeName)
    {
        MeshTexture texture;
        {
            bool skip = false;
            for(unsigned int j = 0; j < this->TexturesLoaded.size(); j++)
            {
                if(strcmp(this->TexturesLoaded[j].path.data(), texPath.c_str()) == 0)
                {
                    texture = this->TexturesLoaded[j];
                    skip = true;
                    break;
                }
            }
            if(!skip)
            {
                texture.id = TextureFromFile(texPath.c_str(), this->Directory, &texture.data);
                texture.type = typeName;
                texture.path = texPath.c_str();
                this->TexturesLoaded.push_back(texture);
            }
        }
        return texture;
    }
};

unsigned int TextureFromFile(const char *path, const std::string &directory, unsigned char** textureData, bool gamma)
{
    std::string filename = std::string(path);
    filename = directory + '/' + filename;

    unsigned int textureID;
    Renderer->GenTextures(1, &textureID);

    int width, height, nrComponents;
    unsigned char *data = stbi_load(filename.c_str(), &width, &height, &nrComponents, 0);
    if (data)
    {
        TextureFormat format;
        if (nrComponents == 1)
            format = RED;
        else if (nrComponents == 3)
            format = RGB;
        else if (nrComponents == 4)
            format = RGBA;

        Renderer->BindTexture(Texture2D, textureID);
        Renderer->TexImage2D(Texture2D, 0, format, width, height, format, Byte_Type, data);
        //glGenerateMipmap(GL_TEXTURE_2D);

        //stbi_image_free(data);
        *textureData = data;
    }
    else
    {
        stbi_image_free(data);
    }

    return textureID;
}
#endif