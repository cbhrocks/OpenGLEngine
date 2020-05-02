#pragma once

#include <glad/glad.h>
#include <glm/glm.hpp>

#include <string>
#include <fstream>
#include <sstream>
#include <iostream>

class Shader
{
    public:
		class builder;
		Shader();
		// Constructor generates the shader on the fly
		Shader(const GLchar* vertexPath, const GLchar* fragmentPath, const GLchar* geometryPath = nullptr);

        // Uses the current shader
		const void Use() const;
		const GLuint getId() const;

		// utility uniform functions
		// ------------------------------------------------------------------------
		const Shader& setBool(const std::string &name, bool value) const;
		// ------------------------------------------------------------------------
		const Shader& setInt(const std::string &name, int value) const;
		// ------------------------------------------------------------------------
		const Shader& setFloat(const std::string &name, float value) const;
		// ------------------------------------------------------------------------
		const Shader& setVec2(const std::string &name, const glm::vec2 &value) const;
		const Shader& setVec2(const std::string &name, float x, float y) const;
		// ------------------------------------------------------------------------
		const Shader& setVec3(const std::string &name, const glm::vec3 &value) const;
		const Shader& setVec3(const std::string &name, float x, float y, float z) const;
		// ------------------------------------------------------------------------
		const Shader& setVec4(const std::string &name, const glm::vec4 &value) const;
		const Shader& setVec4(const std::string &name, float x, float y, float z, float w) const;
		// ------------------------------------------------------------------------
		const Shader& setMat2(const std::string &name, const glm::mat2 &mat) const;
		// ------------------------------------------------------------------------
		const Shader& setMat3(const std::string &name, const glm::mat3 &mat) const;
		// ------------------------------------------------------------------------
		const Shader& setMat4(const std::string &name, const glm::mat4 &mat) const;
		// ------------------------------------------------------------------------
		const Shader& setUniformBlock(const std::string &name, const GLuint &binding) const;
		const GLuint getUniformBlockSize(const std::string &name) const;
		const GLuint Shader::getUniformOffset(const std::string &name) const;

    private:
        GLuint Program;

        // utility function for checking shader compilation/linking errors.
        // ------------------------------------------------------------------------
		void checkCompileErrors(GLuint shader, std::string type);
};
