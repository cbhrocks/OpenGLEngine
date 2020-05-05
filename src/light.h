#pragma once

#include <glad/glad.h> // holds all OpenGL type declarations

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include <string>
#include <fstream>
#include <sstream>
#include <iostream>
#include <vector>
#include <functional>
#include <map>

#include "model.h"
#include "shader.h"
#include "glHelper.h"
#include "vertexData.h"
#include "TextureManager.h"
#include "ShadowMap.h"
#include "ShadowCubeMap.h"

class ILight {
public:
	virtual void uploadUniforms(const Shader& shader, const std::string& lightNum) const = 0;
	virtual GLuint updateUniformBlock(GLuint ubo, GLuint start) = 0;
};

class Light: public ILight
{
    public:
		Light(
			glm::vec3 color,
			float ambient,
			float diffuse,
			float specular,
			const std::string prefix
		);
        /*  Model Data */
		const glm::vec3 getColor() const { return this->color; }
		void getColor(const glm::vec3& color) { this->color = color; }
		const float getAmbient() const { return this->ambient; }
		void setAmbient(const float& ambient) { this->ambient = ambient; }
		const float getDiffuse() const { return this->diffuse; }
		void setDiffuse(const float& diffuse) { this->diffuse = diffuse; }
		const float getSpecular() const { return this->specular; }
		void setSpecular(const float& specular) { this->specular = specular; }
		
		virtual void uploadUniforms(const Shader& shader, const std::string& lightNum) const = 0;
		virtual GLuint updateUniformBlock(GLuint ubo, GLuint start) = 0;

    protected:
		glm::vec3 color;
		float ambient;
		float diffuse;
		float specular;
		const std::string prefix;
		GLuint shadowFBO;
		Texture shadowTexture;
		std::map<std::string, std::function<void(Light*)>> updateFuncs;
};

class PointLight : public Light
{
    public:

		PointLight(
			glm::vec3 position,
			glm::vec3 color,
			float ambient,
			float diffuse,
			float specular,
			float constant,
			float linear,
			float quadratic,
			const std::string prefix = std::string("p")
		);

		const float getConstant() const { return this->constant; }
		void setConstant(const float constant) { this->constant = constant; }
		const float getLinear() const { return this->linear; };
		void setLinear(const float linear) { this->linear = linear; }
		const float getQuadratic() const { return this->quadratic; }
		void setQuadratic(const float quadratic) { this->quadratic = quadratic; }
		const glm::vec3 getPosition() { return this->position; }
		void setPosition(const glm::vec3 position) { this->position = position; }
		const Model* getModel() { return this->model; }
		void setModel(Model* model) { this->model = model; }

		void addUpdateFunction(const std::string& name, std::function<void(PointLight*)> lf);
		void runUpdateFuncs();

		void uploadUniforms(const Shader& shader, const std::string& lightNum) const;
		GLuint updateUniformBlock(GLuint ubo, GLuint start);

protected:
	float constant, linear, quadratic;
	glm::vec3 position;
	Model* model;
	std::map<std::string, std::function<void(PointLight*)>> updateFuncs;
};

class DirectionLight : public Light
{
    public:
		DirectionLight(
			glm::vec3 color,
			glm::vec3 direction,
			float ambient,
			float diffuse,
			float specular,
			const std::string prefix = std::string("d")
		);

		glm::vec3 getDirection() const { return this->direction; }
		const glm::vec3& getDirectionRef() const { return this->direction; }
		void setDirection(glm::vec3 direction) { this->direction = direction; }

		void addUpdateFunction(const std::string& name, std::function<void(DirectionLight*)> lf);
		void runUpdateFuncs();

		void uploadUniforms(const Shader& shader, const std::string& lightNum) const;
		GLuint updateUniformBlock(GLuint ubo, GLuint start);

protected:
        glm::vec3 direction;
		std::map<std::string, std::function<void(DirectionLight*)>> updateFuncs;
};

class SpotLight : public PointLight
{
    public:
        glm::vec3 direction;
        float cutOff;
        float outerCutOff;

		SpotLight(
			glm::vec3 position,
			glm::vec3 direction,
			glm::vec3 color,
			float ambient,
			float diffuse,
			float specular,
			float constant,
			float linear,
			float quadratic,
			float cutOff,
			float outerCutOff,
			const std::string prefix = std::string("s")
		);

		glm::vec3 getDirection() const { return this->direction; }
		void setDirection(glm::vec3 direction) { this->direction = direction; }

		float getCutOff() const { return this->cutOff; }
		void setCutOff(float cutOff) { this->cutOff = cutOff; }

		float getOuterCutOff() const { return this->outerCutOff; }
		void setOuterCutOff(float outerCutOff) { this->outerCutOff = outerCutOff; }

		void addUpdateFunction(const std::string& name, std::function<void(SpotLight*)> lf);

		void runUpdateFuncs();

		void uploadUniforms(const Shader& shader, const std::string& lightNum) const;

		GLuint updateUniformBlock(GLuint ubo, GLuint start) override;

private:
	std::map<std::string, std::function<void(SpotLight*)>> updateFuncs;
};


