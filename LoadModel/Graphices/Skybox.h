#pragma once
#include <glad/glad.h>
#include "Shader.h"
#include <vector>
#include "Camera.h"
#include "Marcros.h"
class Skybox
{
public:
	Skybox();
	~Skybox();
	void render(Camera& camera);
private:
	unsigned int VAO, VBO;
	unsigned int textureID;
	Shader shader;
	std::vector<std::string> faces;
    void loadCubemap(std::vector<std::string> faces);
};

