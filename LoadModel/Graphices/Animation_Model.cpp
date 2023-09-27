#include "Animation_Model.h"


void AnimationModel::getAnimationName()
{
    for (int i = 0; i < m_pScene->mNumAnimations; i++) {
		aiAnimation* animation = m_pScene->mAnimations[i];
		cout << "Animation Name : " << animation->mName.C_Str() << " index : " << i << endl;
	}
}

void AnimationModel::Draw(Shader& shader)
{
    for (unsigned int i = 0; i < m_meshes.size(); i++)
        m_meshes[i].Draw(shader);
}

void AnimationModel::getBoneTransform(float TimeInSeconds, vector<glm::mat4>& transforms, int state)
{
    if (state >= maxAnimationIndex) {
        throw std::out_of_range("state out of ranges");
    }
	m_state = state;
	glm::mat4 Identity = glm::mat4(1.0f);
	float TicksPerSecond = (float)(m_pScene->mAnimations[m_state]->mTicksPerSecond != 0 ? m_pScene->mAnimations[m_state]->mTicksPerSecond : 25.0f);
	float TimeInTicks = TimeInSeconds * TicksPerSecond;
	float AnimationTime = fmod(TimeInTicks, (float)m_pScene->mAnimations[m_state]->mDuration);
	ReadNodeHeirarchy(AnimationTime, m_pScene->mRootNode, Identity);
	transforms.resize(m_NumBones);
	for (unsigned int i = 0; i < m_NumBones; i++) {
		transforms[i] = m_BoneInfo[i].FinalTransformation;
	}
}

void AnimationModel::loadModel(string const& path)
{
	m_pScene = Importer.ReadFile(path, aiProcess_Triangulate | aiProcess_GenSmoothNormals | aiProcess_FlipUVs | aiProcess_CalcTangentSpace);
	if (!m_pScene || m_pScene->mFlags & AI_SCENE_FLAGS_INCOMPLETE || !m_pScene->mRootNode) {
		cout << "ERROR::ASSIMP:: " << Importer.GetErrorString() << endl;
		return;
	}
	GlobalInverseTransform = AssimpToGlmMatrix(m_pScene->mRootNode->mTransformation);
	GlobalInverseTransform = glm::inverse(GlobalInverseTransform);
	directory = path.substr(0, path.find_last_of('/'));
    maxAnimationIndex = m_pScene->mNumAnimations;
	processNode(m_pScene->mRootNode);
}

void AnimationModel::processNode(aiNode* node)
{
	for (unsigned int i = 0; i < node->mNumMeshes; i++) {
		aiMesh* mesh = m_pScene->mMeshes[node->mMeshes[i]];
		m_meshes.push_back(processMesh(mesh));
	}
	for (unsigned int i = 0; i < node->mNumChildren; i++) {
		processNode(node->mChildren[i]);
	}
}

AnimationMesh AnimationModel::processMesh(aiMesh* mesh)
{
    vector<AnimationVertex> vertices;
    vector<unsigned int> indices;
    vector<AnimationTexture> textures;
    vertices.resize(mesh->mNumVertices);
    indices.reserve(mesh->mNumFaces * 3);
    for (unsigned int i = 0; i < mesh->mNumVertices; i++) {
        AnimationVertex& vertex = vertices[i];
        glm::vec3 vector;
        vector.x = mesh->mVertices[i].x;
        vector.y = mesh->mVertices[i].y;
        vector.z = mesh->mVertices[i].z;
        vertex.Position = vector;
        if (mesh->HasNormals()) {
            vector.x = mesh->mNormals[i].x;
            vector.y = mesh->mNormals[i].y;
            vector.z = mesh->mNormals[i].z;
            vertex.Normal = vector;
        }
        if (mesh->mTextureCoords[0]) {
            glm::vec2 vec;
            vec.x = mesh->mTextureCoords[0][i].x;
            vec.y = mesh->mTextureCoords[0][i].y;
            vertex.TexCoords = vec;
            vector.x = mesh->mTangents[i].x;
            vector.y = mesh->mTangents[i].y;
            vector.z = mesh->mTangents[i].z;
            vertex.Tangent = vector;
            vector.x = mesh->mBitangents[i].x;
            vector.y = mesh->mBitangents[i].y;
            vector.z = mesh->mBitangents[i].z;
            vertex.Bitangent = vector;
        }
        else
            vertex.TexCoords = glm::vec2(0.0f, 0.0f);
    }
    for (unsigned int j = 0; j < mesh->mNumBones; ++j) {
        unsigned int boneindex = 0;
        string bonename(mesh->mBones[j]->mName.data);
        if (m_BoneMapping.find(bonename) == m_BoneMapping.end()) {
            boneindex = m_NumBones;
            m_NumBones++;
            BoneInfo bi;
            m_BoneInfo.push_back(bi);
        }
        else {
            boneindex = m_BoneMapping[bonename];
        }
        m_BoneMapping[bonename] = boneindex;
        m_BoneInfo[boneindex].OffsetMatrix = AssimpToGlmMatrix(mesh->mBones[j]->mOffsetMatrix);
        for (unsigned int k = 0; k < mesh->mBones[j]->mNumWeights; ++k) {
            unsigned int VertexID = mesh->mBones[j]->mWeights[k].mVertexId;
            float Weight = mesh->mBones[j]->mWeights[k].mWeight;
            vertices[VertexID].AddBoneData(boneindex, Weight);
        }
    }
    for (unsigned int i = 0; i < mesh->mNumFaces; i++)
    {
        aiFace face = mesh->mFaces[i];
        for (unsigned int j = 0; j < face.mNumIndices; j++)
            indices.push_back(face.mIndices[j]);
    }
    aiMaterial* material = m_pScene->mMaterials[mesh->mMaterialIndex];
    vector<AnimationTexture> diffuseMaps = loadMaterialTextures(material, aiTextureType_DIFFUSE, "texture_diffuse");
    textures.insert(textures.end(), diffuseMaps.begin(), diffuseMaps.end());
    vector<AnimationTexture> specularMaps = loadMaterialTextures(material, aiTextureType_SPECULAR, "texture_specular");
    textures.insert(textures.end(), specularMaps.begin(), specularMaps.end());
    std::vector<AnimationTexture> normalMaps = loadMaterialTextures(material, aiTextureType_HEIGHT, "texture_normal");
    textures.insert(textures.end(), normalMaps.begin(), normalMaps.end());
    std::vector<AnimationTexture> heightMaps = loadMaterialTextures(material, aiTextureType_AMBIENT, "texture_height");
    textures.insert(textures.end(), heightMaps.begin(), heightMaps.end());
    return AnimationMesh(vertices, indices, textures);
}

vector<AnimationTexture> AnimationModel::loadMaterialTextures(aiMaterial* mat, aiTextureType type, string typeName)
{
    vector<AnimationTexture> textures;
    for (unsigned int i = 0; i < mat->GetTextureCount(type); i++)
    {
        aiString str;
        mat->GetTexture(type, i, &str);
        bool skip = false;
        for (unsigned int j = 0; j < textures_loaded.size(); j++)
        {
            if (std::strcmp(textures_loaded[j].path.data(), str.C_Str()) == 0)
            {
                textures.push_back(textures_loaded[j]);
                skip = true;
                break;
            }
        }
        if (!skip) {
            AnimationTexture texture;
            texture.id = AnimationTextureFromFile(directory, str.C_Str());
            texture.type = typeName;
            texture.path = str.C_Str();
            textures.push_back(texture);
            textures_loaded.push_back(texture);
        }
    }
    return textures;
}

void AnimationModel::ReadNodeHeirarchy(float AnimationTime, const aiNode* pNode, const glm::mat4& ParentTransform)
{
    string NodeName(pNode->mName.data);
    const aiAnimation* pAnimation = m_pScene->mAnimations[m_state];
    glm::mat4 NodeTransformation = AssimpToGlmMatrix(pNode->mTransformation);
    const aiNodeAnim* pNodeAnim = FindNodeAnim(pAnimation, NodeName);
    if (pNodeAnim) {
        glm::vec3 Scaling;
        CalcInterpolatedScaling(Scaling, AnimationTime, pNodeAnim);
        glm::mat4 ScalingM(1.0f);
        ScalingM = glm::scale(ScalingM, Scaling);
        glm::quat RotationQ;
        CalcInterpolatedRotation(RotationQ, AnimationTime, pNodeAnim);
        glm::mat4 RotationM = glm::mat4(RotationQ);
        glm::vec3 Translation;
        CalcInterpolatedPosition(Translation, AnimationTime, pNodeAnim);
        glm::mat4 TranslationM(1.0f);
        TranslationM = glm::translate(TranslationM, Translation);
        NodeTransformation = TranslationM * RotationM * ScalingM;
    }
    glm::mat4 GlobalTransformation = ParentTransform * NodeTransformation;
    if (m_BoneMapping.find(NodeName) != m_BoneMapping.end()) {
        unsigned int BoneIndex = m_BoneMapping[NodeName];
        m_BoneInfo[BoneIndex].FinalTransformation = GlobalInverseTransform * GlobalTransformation * m_BoneInfo[BoneIndex].OffsetMatrix;
    }
    for (unsigned int i = 0; i < pNode->mNumChildren; ++i) {

        ReadNodeHeirarchy(AnimationTime, pNode->mChildren[i], GlobalTransformation);
    }
}

const aiNodeAnim* AnimationModel::FindNodeAnim(const aiAnimation* pAnimation, const string NodeName)
{
    for (unsigned int i = 0; i < pAnimation->mNumChannels; ++i) {
        const aiNodeAnim* pNodeAnim = pAnimation->mChannels[i];
        if (string(pNodeAnim->mNodeName.data) == NodeName) {
            return pNodeAnim;
        }
    }
    return NULL;
}

void AnimationModel::CalcInterpolatedScaling(glm::vec3& Out, float AnimationTime, const aiNodeAnim* pNodeAnim)
{
    if (pNodeAnim->mNumScalingKeys == 1) {
        const aiVector3D& scale = pNodeAnim->mScalingKeys[0].mValue;
        Out = glm::vec3(scale.x, scale.y, scale.z);
        return;
    }
    unsigned int ScalingIndex = FindScaling(AnimationTime, pNodeAnim);
    unsigned int NextScalingIndex = (ScalingIndex + 1);
    assert(NextScalingIndex < pNodeAnim->mNumScalingKeys);
    float DeltaTime = (float)(pNodeAnim->mScalingKeys[NextScalingIndex].mTime - pNodeAnim->mScalingKeys[ScalingIndex].mTime);
    float Factor = (AnimationTime - (float)pNodeAnim->mScalingKeys[ScalingIndex].mTime) / DeltaTime;
    assert(Factor >= 0.0f && Factor <= 1.0f);
    const aiVector3D& Start = pNodeAnim->mScalingKeys[ScalingIndex].mValue;
    const aiVector3D& End = pNodeAnim->mScalingKeys[NextScalingIndex].mValue;
    glm::vec3 glmStart(Start.x, Start.y, Start.z);
    glm::vec3 glmEnd(End.x, End.y, End.z);
    glm::vec3 Delta = glmEnd - glmStart;
    Out = glmStart + Factor * Delta;
}

unsigned int AnimationModel::FindScaling(float AnimationTime, const aiNodeAnim* pNodeAnim)
{
    assert(pNodeAnim->mNumScalingKeys > 0);
    for (unsigned int i = 0; i < pNodeAnim->mNumScalingKeys - 1; i++) {
        if (AnimationTime < (float)pNodeAnim->mScalingKeys[i + 1].mTime) {
            return i;
        }
    }
    assert(0);
    return 0;
}

void AnimationModel::CalcInterpolatedRotation(glm::quat& Out, float AnimationTime, const aiNodeAnim* pNodeAnim)
{
    if (pNodeAnim->mNumRotationKeys == 1) {
        Out = glm::quat(pNodeAnim->mRotationKeys[0].mValue.w, pNodeAnim->mRotationKeys[0].mValue.x,
            pNodeAnim->mRotationKeys[0].mValue.y, pNodeAnim->mRotationKeys[0].mValue.z);
        return;
    }
    unsigned int RotationIndex = FindRotation(AnimationTime, pNodeAnim);
    unsigned int NextRotationIndex = (RotationIndex + 1);
    assert(NextRotationIndex < pNodeAnim->mNumRotationKeys);
    float DeltaTime = (float)(pNodeAnim->mRotationKeys[NextRotationIndex].mTime - pNodeAnim->mRotationKeys[RotationIndex].mTime);
    float Factor = (AnimationTime - (float)pNodeAnim->mRotationKeys[RotationIndex].mTime) / DeltaTime;
    assert(Factor >= 0.0f && Factor <= 1.0f);
    const aiQuaternion& StartRotationQ = pNodeAnim->mRotationKeys[RotationIndex].mValue;
    const aiQuaternion& EndRotationQ = pNodeAnim->mRotationKeys[NextRotationIndex].mValue;
    glm::quat glmStartRotationQ(StartRotationQ.w, StartRotationQ.x, StartRotationQ.y, StartRotationQ.z);
    glm::quat glmEndRotationQ(EndRotationQ.w, EndRotationQ.x, EndRotationQ.y, EndRotationQ.z);
    Out = glm::slerp(glmStartRotationQ, glmEndRotationQ, Factor);
}
void AnimationModel::CalcInterpolatedPosition(glm::vec3& Out, float AnimationTime, const aiNodeAnim* pNodeAnim)
{
    if (pNodeAnim->mNumPositionKeys == 1) {
        const aiVector3D& pos = pNodeAnim->mPositionKeys[0].mValue;
        Out = glm::vec3(pos.x, pos.y, pos.z);
        return;
    }
    unsigned int PositionIndex = FindPosition(AnimationTime, pNodeAnim);
    unsigned int NextPositionIndex = (PositionIndex + 1);
    assert(NextPositionIndex < pNodeAnim->mNumPositionKeys);
    float DeltaTime = (float)(pNodeAnim->mPositionKeys[NextPositionIndex].mTime - pNodeAnim->mPositionKeys[PositionIndex].mTime);
    float Factor = (AnimationTime - (float)pNodeAnim->mPositionKeys[PositionIndex].mTime) / DeltaTime;
    assert(Factor >= 0.0f && Factor <= 1.0f);
    const aiVector3D& Start = pNodeAnim->mPositionKeys[PositionIndex].mValue;
    const aiVector3D& End = pNodeAnim->mPositionKeys[NextPositionIndex].mValue;
    glm::vec3 glmStart(Start.x, Start.y, Start.z);
    glm::vec3 glmEnd(End.x, End.y, End.z);
    glm::vec3 Delta = glmEnd - glmStart;
    Out = glmStart + Factor * Delta;
}
unsigned int AnimationModel::FindPosition(float AnimationTime, const aiNodeAnim* pNodeAnim)
{
    for (unsigned int i = 0; i < pNodeAnim->mNumPositionKeys - 1; i++) {
        if (AnimationTime < (float)pNodeAnim->mPositionKeys[i + 1].mTime) {
            return i;
        }
    }
    assert(0);
    return 0;
}
unsigned int AnimationModel::FindRotation(float AnimationTime, const aiNodeAnim* pNodeAnim)
{
    assert(pNodeAnim->mNumRotationKeys > 0);
    for (unsigned int i = 0; i < pNodeAnim->mNumRotationKeys - 1; i++) {
        if (AnimationTime < (float)pNodeAnim->mRotationKeys[i + 1].mTime) {
            return i;
        }
    }
    assert(0);
    return 0;
}
unsigned int AnimationTextureFromFile(string directory, const char* path, bool gamma)
{
    string filename = string(path);

    filename = directory + '/' + filename;
//    std::cout << "load: " << filename << std::endl;
    unsigned int textureID;
    glGenTextures(1, &textureID);

    int width, height, nrComponents;
    unsigned char* data = stbi_load(filename.c_str(), &width, &height, &nrComponents, 0);
    if (data)
    {
        GLenum format;
        if (nrComponents == 1)
            format = GL_RED;
        else if (nrComponents == 3)
            //            format = GL_RGB; 
            format = GL_RGB;
        else if (nrComponents == 4)
            format = GL_RGBA;
        glBindTexture(GL_TEXTURE_2D, textureID);
        glTexImage2D(GL_TEXTURE_2D, 0, format, width, height, 0, format, GL_UNSIGNED_BYTE, data);
        glGenerateMipmap(GL_TEXTURE_2D);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

        stbi_image_free(data);
    }
    else
    {
        std::cout << "Texture failed to load at path: " << path << std::endl;
        stbi_image_free(data);
    }

    return textureID;
}
glm::mat4 AssimpToGlmMatrix(const aiMatrix4x4& from)
{
    glm::mat4 to;
    to[0][0] = from.a1; to[1][0] = from.a2; to[2][0] = from.a3; to[3][0] = from.a4;
    to[0][1] = from.b1; to[1][1] = from.b2; to[2][1] = from.b3; to[3][1] = from.b4;
    to[0][2] = from.c1; to[1][2] = from.c2; to[2][2] = from.c3; to[3][2] = from.c4;
    to[0][3] = from.d1; to[1][3] = from.d2; to[2][3] = from.d3; to[3][3] = from.d4;
    return to;
    //return glm::transpose(to);
}
