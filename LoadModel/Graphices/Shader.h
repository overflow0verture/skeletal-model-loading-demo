#pragma once
#include <glad/glad.h>;
#include <GLFW/glfw3.h>
#include <iostream>
#include <sstream>
#include <fstream>
#include <string>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

class Shader
{
public:
	~Shader();
	void compile(const char* vertexPath, const char* fragmentPath);
	void bind();
	void unbind();
	inline int getID() { return Program_id; }
	void setFloat(const GLchar* name, GLfloat value, GLboolean useShader = false);
	void setInteger(const GLchar* name, GLint value, GLboolean useShader = false);
	void setVector2f(const GLchar* name, GLfloat x, GLfloat y, GLboolean useShader = false);
	void setVector2f(const GLchar* name, const glm::vec2& value, GLboolean useShader = false);
	void setVector3f(const GLchar* name, GLfloat x, GLfloat y, GLfloat z, GLboolean useShader = false);
	void setVector3f(const GLchar* name, const glm::vec3& value, GLboolean useShader = false);
	void setVector4f(const GLchar* name, GLfloat x, GLfloat y, GLfloat z, GLfloat w, GLboolean useShader = false);
	void setVector4f(const GLchar* name, const glm::vec4& value, GLboolean useShader = false);
	void setMatrix4(const GLchar* name, const glm::mat4& matrix, GLboolean useShader = false);
private:
	unsigned int Program_id;
	const GLchar* readShaderFile(const char* filePath); // return code
	void getGlerror(unsigned int ID, GLenum op) const;
};

