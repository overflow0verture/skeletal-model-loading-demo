#pragma once

#include <glad/glad.h>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>


#include <assimp/Importer.hpp>
#include <assimp/scene.h>
#include <assimp/postprocess.h>

#include "Animation_Mesh.h"
#include "Shader.h"

#include <string>
#include <fstream>
#include <sstream>
#include <iostream>
#include <map>
#include <vector>

#include <stdexcept>

using namespace std; 


struct BoneInfo {
    glm::mat4 OffsetMatrix;
    glm::mat4 FinalTransformation;
    BoneInfo() {}
    BoneInfo(const glm::mat4& Offset) {
        OffsetMatrix = Offset;
        FinalTransformation = glm::mat4(1.0f);
    }
};

unsigned int AnimationTextureFromFile(string directory, const char* path, bool gamma = false);

glm::mat4 AssimpToGlmMatrix(const aiMatrix4x4& from);

class AnimationModel
{
public: 
    vector<AnimationTexture> textures_loaded;
    vector<AnimationMesh> m_meshes;
    string directory;
    int m_state;
    bool gammaCorrection; 
    AnimationModel(string const& path, bool gamma = false) : gammaCorrection(gamma) 
    {
        loadModel(path);
    } 
    
    void Draw(Shader& shader);
    void getBoneTransform(float TimeInSeconds, vector<glm::mat4>& transforms, int state);
    void getAnimationName();
private: 
    glm::mat4 GlobalInverseTransform;
    vector<BoneInfo> m_BoneInfo;
    map<string, unsigned int> m_BoneMapping;
    Assimp::Importer Importer;
    const aiScene* m_pScene;
    unsigned int m_NumBones = 0;
    unsigned int maxAnimationIndex = 0;
    
    void loadModel(string const& path);

    void processNode(aiNode* node);

    AnimationMesh processMesh(aiMesh* mesh);

    vector<AnimationTexture> loadMaterialTextures(aiMaterial* mat, aiTextureType type, string typeName);

    void ReadNodeHeirarchy(float AnimationTime, const aiNode* pNode, const glm::mat4& ParentTransform);

    const aiNodeAnim* FindNodeAnim(const aiAnimation* pAnimation, const string NodeName);

    void CalcInterpolatedScaling(glm::vec3& Out, float AnimationTime, const aiNodeAnim* pNodeAnim);

    unsigned int FindScaling(float AnimationTime, const aiNodeAnim* pNodeAnim);

    void CalcInterpolatedRotation(glm::quat& Out, float AnimationTime, const aiNodeAnim* pNodeAnim);

    void CalcInterpolatedPosition(glm::vec3& Out, float AnimationTime, const aiNodeAnim* pNodeAnim);

    unsigned int FindPosition(float AnimationTime, const aiNodeAnim* pNodeAnim);

    unsigned int FindRotation(float AnimationTime, const aiNodeAnim* pNodeAnim); 

};


