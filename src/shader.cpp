#include "shader.h"

Shader::Shader() {
}
// Constructor generates the shader on the fly
Shader::Shader(const GLchar* vertexPath, const GLchar* fragmentPath, const GLchar* geometryPath)
{
	// 1. Retrieve the vertex/fragment source code from filePath
	std::string vertexCode;
	std::string fragmentCode;
	std::string geometryCode;
	std::ifstream vShaderFile;
	std::ifstream fShaderFile;
	std::ifstream gShaderFile;
	// ensures ifstream objects can throw exceptions:
	vShaderFile.exceptions(std::ifstream::badbit);
	fShaderFile.exceptions(std::ifstream::badbit);
	gShaderFile.exceptions(std::ifstream::badbit);
	try
	{
		// Open files
		vShaderFile.open(vertexPath);
		fShaderFile.open(fragmentPath);
		std::stringstream vShaderStream, fShaderStream;
		// Read file's buffer contents into streams
		vShaderStream << vShaderFile.rdbuf();
		fShaderStream << fShaderFile.rdbuf();
		// close file handlers
		vShaderFile.close();
		fShaderFile.close();
		// Convert stream into string
		vertexCode = vShaderStream.str();
		fragmentCode = fShaderStream.str();

		if (geometryPath != nullptr)
		{
			gShaderFile.open(geometryPath);
			std::stringstream gShaderStream;
			gShaderStream << gShaderFile.rdbuf();
			gShaderFile.close();
			geometryCode = gShaderStream.str();
		}
	}
	catch (std::ifstream::failure e)
	{
		std::cout << "ERROR::SHADER::FILE_NOT_SUCCESFULLY_READ" << std::endl;
	}

	const GLchar* vShaderCode = vertexCode.c_str();
	const GLchar* fShaderCode = fragmentCode.c_str();

	// 2. Compile shaders
	GLuint vertex, fragment, geometry;
	GLint success;
	GLchar infoLog[512];

	// Vertex Shader
	vertex = glCreateShader(GL_VERTEX_SHADER);
	glShaderSource(vertex, 1, &vShaderCode, NULL);
	glCompileShader(vertex);
	// Print compile errors if any
	checkCompileErrors(vertex, "VERTEX");

	// Fragment Shader
	fragment = glCreateShader(GL_FRAGMENT_SHADER);
	glShaderSource(fragment, 1, &fShaderCode, NULL);
	glCompileShader(fragment);
	// Print compile errors if any
	checkCompileErrors(fragment, "FRAGMENT");

	if (geometryPath != nullptr)
	{
		const GLchar* gShaderCode = geometryCode.c_str();
		// GeometryShader
		geometry = glCreateShader(GL_GEOMETRY_SHADER);
		glShaderSource(geometry, 1, &gShaderCode, NULL);
		glCompileShader(geometry);
		// Print compile errors if any
		checkCompileErrors(geometry, "GEOMETRY");
	}

	// Shader Program
	this->Program = glCreateProgram();
	glAttachShader(this->Program, vertex);
	glAttachShader(this->Program, fragment);

	if (geometryPath != nullptr)
	{
		glAttachShader(this->Program, geometry);
	}
	glLinkProgram(this->Program);
	// Print linking errors if any
	checkCompileErrors(this->Program, "PROGRAM");

	// Delete the shaders as they're linked into our program now and no longer necessery
	glDeleteShader(vertex);
	glDeleteShader(fragment);

	if (geometryPath != nullptr)
	{
		glDeleteShader(geometry);
	}

}

// Uses the current shader
const void Shader::Use() const
{
	glUseProgram(this->Program);
}
const GLuint Shader::getId() const
{
	return this->Program;
}

// utility uniform functions
// ------------------------------------------------------------------------
const Shader& Shader::setBool(const std::string &name, bool value) const
{
	glUniform1i(glGetUniformLocation(this->Program, name.c_str()), (int)value);
	return *this;
}
// ------------------------------------------------------------------------
const Shader& Shader::setInt(const std::string &name, int value) const
{
	glUniform1i(glGetUniformLocation(this->Program, name.c_str()), value);
	return *this;
}
// ------------------------------------------------------------------------
const Shader& Shader::setFloat(const std::string &name, float value) const
{
	glUniform1f(glGetUniformLocation(this->Program, name.c_str()), value);
	return *this;
}
// ------------------------------------------------------------------------
const Shader& Shader::setVec2(const std::string &name, const glm::vec2 &value) const
{
	glUniform2fv(glGetUniformLocation(this->Program, name.c_str()), 1, &value[0]);
	return *this;
}
const Shader& Shader::setVec2(const std::string &name, float x, float y) const
{
	glUniform2f(glGetUniformLocation(this->Program, name.c_str()), x, y);
	return *this;
}
// ------------------------------------------------------------------------
const Shader& Shader::setVec3(const std::string &name, const glm::vec3 &value) const
{
	glUniform3fv(glGetUniformLocation(this->Program, name.c_str()), 1, &value[0]);
	return *this;
}
const Shader& Shader::setVec3(const std::string &name, float x, float y, float z) const
{
	glUniform3f(glGetUniformLocation(this->Program, name.c_str()), x, y, z);
	return *this;
}
// ------------------------------------------------------------------------
const Shader& Shader::setVec4(const std::string &name, const glm::vec4 &value) const
{
	glUniform4fv(glGetUniformLocation(this->Program, name.c_str()), 1, &value[0]);
	return *this;
}
const Shader& Shader::setVec4(const std::string &name, float x, float y, float z, float w) const
{
	glUniform4f(glGetUniformLocation(this->Program, name.c_str()), x, y, z, w);
	return *this;
}
// ------------------------------------------------------------------------
const Shader& Shader::setMat2(const std::string &name, const glm::mat2 &mat) const
{
	glUniformMatrix2fv(glGetUniformLocation(this->Program, name.c_str()), 1, GL_FALSE, &mat[0][0]);
	return *this;
}
// ------------------------------------------------------------------------
const Shader& Shader::setMat3(const std::string &name, const glm::mat3 &mat) const
{
	glUniformMatrix3fv(glGetUniformLocation(this->Program, name.c_str()), 1, GL_FALSE, &mat[0][0]);
	return *this;
}
// ------------------------------------------------------------------------
const Shader& Shader::setMat4(const std::string &name, const glm::mat4 &mat) const
{
	glUniformMatrix4fv(glGetUniformLocation(this->Program, name.c_str()), 1, GL_FALSE, &mat[0][0]);
	return *this;
}
// ------------------------------------------------------------------------
const Shader& Shader::setUniformBlock(const std::string &name, const GLuint &binding) const
{
	glUniformBlockBinding(this->Program, glGetUniformBlockIndex(this->Program, name.c_str()), binding);
	return *this;
}

// utility function for checking shader compilation/linking errors.
// ------------------------------------------------------------------------
void Shader::checkCompileErrors(GLuint shader, std::string type)
{
	GLint success;
	GLchar infoLog[1024];
	if (type != "PROGRAM")
	{
		glGetShaderiv(shader, GL_COMPILE_STATUS, &success);
		if (!success)
		{
			glGetShaderInfoLog(shader, 1024, NULL, infoLog);
			std::cout << "ERROR::SHADER_COMPILATION_ERROR of type: " << type << "\n" << infoLog << "\n -- --------------------------------------------------- -- " << std::endl;
		}
	}
	else
	{
		glGetProgramiv(shader, GL_LINK_STATUS, &success);
		if (!success)
		{
			glGetProgramInfoLog(shader, 1024, NULL, infoLog);
			std::cout << "ERROR::PROGRAM_LINKING_ERROR of type: " << type << "\n" << infoLog << "\n -- --------------------------------------------------- -- " << std::endl;
		}
	}
}
