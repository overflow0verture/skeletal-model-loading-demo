#pragma once
#include "Animation_Model.h"
#include <memory>
#include "Camera.h"

class Wolf
{
public:
	Wolf(glm::vec3 position);
	void render(Camera& camera, int animation); 
	void render(Camera& camera);
private:
	std::unique_ptr<AnimationModel> m_model; 
	glm::vec3 position;
	Shader shader;
	float createTime;
};