#include "Shader.h"


Shader::~Shader()
{
	glDeleteProgram(Program_id);
}

void Shader::compile(const char* vertexPath, const char* fragmentPath)
{
	std::string vertexCode, fragmentCode;
	std::ifstream vShaderFile, fShaderFile;
	vShaderFile.exceptions(std::ifstream::failbit | std::ifstream::badbit);
	try {
		vShaderFile.open(vertexPath);
		fShaderFile.open(fragmentPath);
		std::stringstream vShaderStream, fShaderStream;
		vShaderStream << vShaderFile.rdbuf();
		fShaderStream << fShaderFile.rdbuf();
		vShaderFile.close();
		fShaderFile.close();
		vertexCode = vShaderStream.str();
		fragmentCode = fShaderStream.str();
	}
	catch (std::ifstream::failure e)
	{
		std::cout << "ERROR::SHADER::FILE_NOT_SUCCESSFULLY_READ" << std::endl;
	}
	const char* vShaderCode = vertexCode.c_str();
	const char* fShaderCode = fragmentCode.c_str();
	unsigned int vertex, fragment;

	vertex = glCreateShader(GL_VERTEX_SHADER);
	glShaderSource(vertex, 1, &vShaderCode, NULL);
	glCompileShader(vertex);
	getGlerror(vertex, GL_COMPILE_STATUS);
	fragment = glCreateShader(GL_FRAGMENT_SHADER);
	glShaderSource(fragment, 1, &fShaderCode, NULL);
	glCompileShader(fragment);
	getGlerror(fragment, GL_COMPILE_STATUS);

	Program_id = glCreateProgram();
	glAttachShader(Program_id, fragment);
	glAttachShader(Program_id, vertex);
	glLinkProgram(Program_id);
	getGlerror(Program_id, GL_LINK_STATUS);
	glDeleteShader(vertex);
	glDeleteShader(fragment);
}
const GLchar* Shader::readShaderFile(const char* filePath)
{
	std::string fileCode;
	std::ifstream fileShaderFile;
	fileShaderFile.exceptions(std::ifstream::failbit | std::ifstream::badbit);
	try {
		fileShaderFile.open(filePath);
		std::stringstream fileShaderStream;
		fileShaderStream << fileShaderFile.rdbuf();
		fileShaderFile.close();
		fileCode = fileShaderStream.str();
	}
	catch (std::ifstream::failure e)
	{
		std::cout << "ERROR::SHADER::FILE_NOT_SUCCESSFULLY_READ" << std::endl;
	}
	return fileCode.c_str();
}

void Shader::getGlerror(unsigned int ID, GLenum op) const
{
	int success;
	char infoLog[512];

	if (op == GL_COMPILE_STATUS) {
		glGetShaderiv(ID, op, &success);
		if (!success) {
			glGetShaderInfoLog(ID, 512, NULL, infoLog);
			std::cout << "ERROR::SHADER::PROGRAM::COMPILATION_FILED\n" << infoLog << std::endl;
		}
	}
	else if (op == GL_LINK_STATUS) {
		glGetProgramiv(ID, op, &success);
		if (!success) {
			glGetProgramInfoLog(ID, 512, NULL, infoLog);
			std::cout << "ERROR::SHADER::PROGRAM::LINKING_FAILED\n" << infoLog << std::endl;
		}
	}
}

void Shader::bind()
{
	glUseProgram(Program_id);
}

void Shader::unbind()
{
	glUseProgram(0);
}

void Shader::setFloat(const GLchar* name, GLfloat value, GLboolean useShader)
{
	if (useShader)
		Shader::bind();
	glUniform1f(glGetUniformLocation(this->Program_id, name), value);
}
void Shader::setInteger(const GLchar* name, GLint value, GLboolean useShader)
{
	if (useShader)
		Shader::bind();
	glUniform1i(glGetUniformLocation(this->Program_id, name), value);
}
void Shader::setVector2f(const GLchar* name, GLfloat x, GLfloat y, GLboolean useShader)
{
	if (useShader)
		Shader::bind();
	glUniform2f(glGetUniformLocation(this->Program_id, name), x, y);
}
void Shader::setVector2f(const GLchar* name, const glm::vec2& value, GLboolean useShader)
{
	if (useShader)
		Shader::bind();
	glUniform2f(glGetUniformLocation(this->Program_id, name), value.x, value.y);
}
void Shader::setVector3f(const GLchar* name, GLfloat x, GLfloat y, GLfloat z, GLboolean useShader)
{
	if (useShader)
		Shader::bind();
	glUniform3f(glGetUniformLocation(this->Program_id, name), x, y, z);
}
void Shader::setVector3f(const GLchar* name, const glm::vec3& value, GLboolean useShader)
{
	if (useShader)
		Shader::bind();
	glUniform3f(glGetUniformLocation(this->Program_id, name), value.x, value.y, value.z);
}
void Shader::setVector4f(const GLchar* name, GLfloat x, GLfloat y, GLfloat z, GLfloat w, GLboolean useShader)
{
	if (useShader)
		Shader::bind();
	glUniform4f(glGetUniformLocation(this->Program_id, name), x, y, z, w);
}
void Shader::setVector4f(const GLchar* name, const glm::vec4& value, GLboolean useShader)
{
	if (useShader)
		Shader::bind();
	glUniform4f(glGetUniformLocation(this->Program_id, name), value.x, value.y, value.z, value.w);
}
void Shader::setMatrix4(const GLchar* name, const glm::mat4& matrix, GLboolean useShader)
{
	if (useShader)
		Shader::bind();
	glUniformMatrix4fv(glGetUniformLocation(this->Program_id, name), 1, GL_FALSE, glm::value_ptr(matrix));
}