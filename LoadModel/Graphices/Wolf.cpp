#include "Wolf.h"
#include "Timer.h"
#include "Marcros.h"
#include <vector>
using T = Timer;

Wolf::Wolf(glm::vec3 position) : position(position)
{
	m_model = std::make_unique<AnimationModel>("./Resources/fbx/Wolf_One_fbx7.4_binary.fbx");
//	shader.compile("./ShaderFile/model.vert", "./ShaderFile/model.frag");
	shader.compile("./ShaderFile/animation_model.vert", "./ShaderFile/animation_model.frag");
	createTime = glfwGetTime();
	m_model->getAnimationName();
}

void Wolf::render(Camera& camera, int animation)
{
	shader.bind(); 
	createTime += T::getInstance().deltaTime;
	glm::mat4 projection = glm::perspective(glm::radians(camera.Zoom), (float)SCR_WIDTH / (float)SCR_HEIGHT, 0.1f, 250.0f);
	glm::mat4 view = camera.GetViewMatrix();
	shader.setMatrix4("projection", projection);
	shader.setMatrix4("view", view);
	glm::mat4 model = glm::mat4(1.0f); 
	model = glm::translate(model, position);
	model = glm::scale(model, glm::vec3(0.008f, 0.008f, 0.008f));

	shader.setMatrix4("model", model); 
	
	vector<glm::mat4> boneTransforms;
	m_model->getBoneTransform(createTime, boneTransforms, animation); 
	for (size_t i = 0; i < boneTransforms.size(); ++i) {
		std::stringstream ss;
		ss << "boneTransforms[" << i << "]";
		std::string uniformName = ss.str();
		shader.setMatrix4(uniformName.c_str(), boneTransforms[i], false);
	}

	m_model->Draw(shader);
	shader.unbind();
}

void Wolf::render(Camera& camera)
{
	shader.bind();
	createTime += T::getInstance().deltaTime;
	glm::mat4 projection = glm::perspective(glm::radians(camera.Zoom), (float)SCR_WIDTH / (float)SCR_HEIGHT, 0.1f, 250.0f);
	glm::mat4 view = camera.GetViewMatrix();
	shader.setMatrix4("projection", projection);
	shader.setMatrix4("view", view);
	glm::mat4 model = glm::mat4(1.0f);
	model = glm::translate(model, glm::vec3(0.0f, 0.0f, 0.0f));
	model = glm::scale(model, glm::vec3(0.0008f, 0.0008f, 0.0008f));
	shader.setMatrix4("model", model); 
	m_model->Draw(shader);
	shader.unbind();
}
