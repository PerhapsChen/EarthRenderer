#pragma once
#include <glad/glad.h> // holds all OpenGL type declarations
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include "shader.h"
//#include "package/glad/src/glad.c"
#include <string>
#include <vector>
#include "texture.h"
using namespace std;

#define MAX_BONE_INFLUENCE 4

struct Vertex {
    // position
    glm::vec3 Position;
    // normal
    glm::vec3 Normal;
    // texCoords
    glm::vec2 TexCoords;
    // tangent
    glm::vec3 Tangent;
    // bitangent
    glm::vec3 Bitangent;
    //bone indexes which will influence this vertex
    int m_BoneIDs[MAX_BONE_INFLUENCE];
    //weights from each bone
    float m_Weights[MAX_BONE_INFLUENCE];
};

class Mesh
{
public:
    vector<Vertex>       vertices;
    vector<unsigned int> indices;
    vector<Texture>      textures;
    unsigned int VAO;

    Mesh(vector<Vertex> vertices, vector<unsigned int> indices, 
                vector<Texture> textures)
    {
        this->vertices = vertices;
        this->indices = indices;
        this->textures = textures;
        // cout <<"Size of Textures Transfer to Mesh: "<< textures.size() << endl;
        for(auto & t : textures){
            cout << t.textureName << endl;
        }
        setupMesh();
        cout << "vectices size: " << vertices.size() << endl;
        cout << "indices size: " << indices.size() << endl;
    }

    void Draw(Shader& shader)
    {
        for (unsigned int i = 0; i < textures.size(); i++)
        {
            glActiveTexture(GL_TEXTURE0 + i); 
            
            glUniform1i(glGetUniformLocation(shader.ID, textures[i].textureName.c_str()), i);
            glBindTexture(GL_TEXTURE_2D, textures[i].textureID);
        }

        // draw mesh
        glBindVertexArray(VAO);
        glDrawElements(GL_TRIANGLES, static_cast<unsigned int>(indices.size()), GL_UNSIGNED_INT, 0);
        glBindVertexArray(0);

        // always good practice to set everything back to defaults once configured.
        glActiveTexture(GL_TEXTURE0);
    }
private:
    // render data 
    unsigned int VBO, EBO;

    // initializes all the buffer objects/arrays
    void setupMesh()
    {
        // create buffers/arrays
        glGenVertexArrays(1, &VAO);
        glGenBuffers(1, &VBO);
        glGenBuffers(1, &EBO);

        glBindVertexArray(VAO);
        // load data into vertex buffers
        glBindBuffer(GL_ARRAY_BUFFER, VBO);
        // A great thing about structs is that their memory layout is sequential for all its items.
        // The effect is that we can simply pass a pointer to the struct and it translates perfectly to a glm::vec3/2 array which
        // again translates to 3/2 floats which translates to a byte array.
        glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(Vertex), &vertices[0], GL_STATIC_DRAW);

        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
        glBufferData(GL_ELEMENT_ARRAY_BUFFER, indices.size() * sizeof(unsigned int), &indices[0], GL_STATIC_DRAW);

        // set the vertex attribute pointers
        // vertex Positions
        glEnableVertexAttribArray(0);
        glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)0);
        // vertex normals
        glEnableVertexAttribArray(1);
        glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, Normal));
        // vertex texture coords
        glEnableVertexAttribArray(2);
        glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, TexCoords));
        // vertex tangent
        glEnableVertexAttribArray(3);
        glVertexAttribPointer(3, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, Tangent));
        // vertex bitangent
        glEnableVertexAttribArray(4);
        glVertexAttribPointer(4, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, Bitangent));
        // ids
        glEnableVertexAttribArray(5);
        glVertexAttribIPointer(5, 4, GL_INT, sizeof(Vertex), (void*)offsetof(Vertex, m_BoneIDs));

        // weights
        glEnableVertexAttribArray(6);
        glVertexAttribPointer(6, 4, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, m_Weights));
        glBindVertexArray(0);
    }

    // void calTangetAndBitangent()
    // {   
    //     //- we have these to calculate tan and bitan
    //     // vector<Vertex>       vertices;
    //     // vector<unsigned int> indices;

    //     for(auto &v : vertices){
    //         v.Tangent = glm::vec3{0};
    //         v.Bitangent = glm::vec3{0};
    //     }

    //     int faceNum = indices.size()/3;
    //     for(int i=0; i<faceNum; i++){
    //         auto &v1 = vertices[indices[3*i+0]];
    //         auto &v2 = vertices[indices[3*i+1]];
    //         auto &v3 = vertices[indices[3*i+2]];
    //         auto pos1 = v1.Position;
    //         auto pos2 = v2.Position;
    //         auto pos3 = v3.Position;
    //         auto uv1 = v1.TexCoords;
    //         auto uv2 = v2.TexCoords;
    //         auto uv3 = v3.TexCoords;
    //         auto edge1 = pos2 - pos1;
    //         auto edge2 = pos3 - pos1;
    //         auto dUV1 = uv2 - uv1;
    //         auto dUV2 = uv3 - uv1;
    //         float f = 1.0f / (dUV1.x * dUV2.y - dUV2.x * dUV1.y);
    //         auto tanx = f * (dUV2.y * edge1.x - dUV1.y * edge2.x);
    //         auto tany = f * (dUV2.y * edge1.y - dUV1.y * edge2.y);
    //         auto tanz = f * (dUV2.y * edge1.z - dUV1.y * edge2.z);
    //         auto bitanx = f * (-dUV2.x * edge1.x - dUV1.x * edge2.x);
    //         auto bitany = f * (-dUV2.x * edge1.y - dUV1.x * edge2.y);
    //         auto bitanz = f * (-dUV2.x * edge1.z - dUV1.x * edge2.z);     
            

    //         // v1.Tangent.x += tanx;
    //         // v1.Tangent.y += tany;
    //         // v1.Tangent.z += tanz;

    //         // v2.Tangent.x += tanx;
    //         // v2.Tangent.y += tany;
    //         // v2.Tangent.z += tanz;
            
    //         // v3.Tangent.x += tanx;
    //         // v3.Tangent.y += tany;
    //         // v3.Tangent.z += tanz;

    //         v1.Tangent += glm::normalize(glm::vec3(tanx,tany,tanz));
    //         v2.Tangent += glm::normalize(glm::vec3(tanx,tany,tanz));
    //         v3.Tangent += glm::normalize(glm::vec3(tanx,tany,tanz));

    //         // v1.Bitangent.x += bitanx;
    //         // v1.Bitangent.y += bitany;
    //         // v1.Bitangent.z += bitanz;
            
    //         // v2.Bitangent.x += bitanx;
    //         // v2.Bitangent.y += bitany;
    //         // v2.Bitangent.z += bitanz;
            
    //         // v3.Bitangent.x += bitanx;
    //         // v3.Bitangent.y += bitany;
    //         // v3.Bitangent.z += bitanz;

    //         v1.Bitangent += glm::normalize(glm::vec3(bitanx,bitany,bitanz));
    //         v2.Bitangent += glm::normalize(glm::vec3(bitanx,bitany,bitanz));
    //         v3.Bitangent += glm::normalize(glm::vec3(bitanx,bitany,bitanz));            
    //     }

    //     for(auto &v : vertices){
    //         v.Tangent = glm::normalize(v.Tangent);
    //         v.Bitangent = glm::normalize(v.Bitangent);
    //     }
        
    // }
};
