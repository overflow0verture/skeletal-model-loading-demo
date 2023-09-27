#pragma once

#include <glad/glad.h>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

#include "Shader.h"
#include "stb_image.h"
#include <string>
#include <vector>
using namespace std;

#define MAX_BONE_INFLUENCE 50

struct AnimationVertex {
    glm::vec3 Position; 
    glm::vec3 Normal; 
    glm::vec2 TexCoords; 
    glm::vec3 Tangent; 
    glm::vec3 Bitangent; 
    int m_BoneIDs[MAX_BONE_INFLUENCE]; 
    float m_Weights[MAX_BONE_INFLUENCE];
    void AddBoneData(unsigned int boneID, float weight) {
        for (unsigned int i = 0; i < MAX_BONE_INFLUENCE; i++) {
            if (m_Weights[i] == 0.0) {
                m_BoneIDs[i] = boneID;
                m_Weights[i] = weight;
                return;
            }
        }
        std::cout << "overflow add max bones" << std::endl;
    }
};

struct AnimationTexture {
    unsigned int id;
    string type;
    string path;
};

class AnimationMesh {
public: 
    vector<AnimationVertex> vertices;
    vector<unsigned int> indices;
    vector<AnimationTexture> textures;
    unsigned int VAO; 
    AnimationMesh(vector<AnimationVertex> vertices, vector<unsigned int> indices, vector<AnimationTexture> textures);
    
    void Draw(Shader& shader);


private: 
    unsigned int VBO, EBO; 
    void setupMesh();
};
