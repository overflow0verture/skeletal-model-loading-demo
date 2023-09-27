#version 330 core
layout (location = 0) in vec3 aPos;
layout (location = 1) in vec3 aNormal;
layout (location = 2) in vec2 aTexCoords;
layout (location = 3) in vec3 aTangent;
layout (location = 4) in vec3 aBitangent;
layout (location = 5) in ivec4 BoneIDs;
layout (location = 6) in vec4 Weights;
out vec2 TexCoords;
out vec3 Normal;
const int MAX_BONES = 200;

uniform mat4 model;
uniform mat4 view;
uniform mat4 projection;
uniform mat4 boneTransforms[MAX_BONES];
void main()
{
    mat4 BoneTransform = boneTransforms[BoneIDs[0]] * Weights[0];
    BoneTransform     += boneTransforms[BoneIDs[1]] * Weights[1];
    BoneTransform     += boneTransforms[BoneIDs[2]] * Weights[2];
    BoneTransform     += boneTransforms[BoneIDs[3]] * Weights[3];

    vec4 PosL =  BoneTransform * vec4(aPos, 1.0); 
    TexCoords = aTexCoords;    
    Normal = aNormal;
    gl_Position = projection * view * model * PosL;
}