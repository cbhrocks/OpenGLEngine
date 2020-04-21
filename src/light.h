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

#include "shader.h"
#include "glHelper.h"
#include "vertexData.h"

//static Shader shadowCubeDepth = Shader("src/shaders/shadowCubeDepth.vert", "src/shaders/shadowCubeDepth.frag");
//checkGLError("Scene::initializeShaders -- shadowDepth");

class Light 
{
    public:
        /*  Model Data */
        glm::vec3 ambient;
        glm::vec3 diffuse;
        glm::vec3 specular;
		const Shader* shader;
		const Shader& shadowDepth;
		const Shader& shadowCubeDepth;
        const std::string prefix;

		Light(
			glm::vec3& ambient,
			glm::vec3& diffuse,
			glm::vec3& specular,
			const Shader* shader,
			const std::string& prefix = std::string(),
			const Shader& shadowDepth = Shader("src/shaders/shadowDepth.vert", "src/shaders/shadowDepth.frag"),
			const Shader& shadowCubeDepth = Shader("src/shaders/shadowDepthCube.vert", "src/shaders/shadowDepthCube.frag", "src/shaders/shadowDepthCube.geom")
		);

		const Shader* getShader() const;

		void setShader(Shader* shader);

		const glm::vec3 getAmbient() const;

		void setAmbient(const glm::vec3& ambient);

		const glm::vec3 getDiffuse() const;

		void setDiffuse(const glm::vec3& diffuse);

		const glm::vec3 getSpecular() const;

		void setSpecular(const glm::vec3& specular);

		virtual void addUpdateFunction(const std::string& name, std::function<void(Light*)> lf);

		virtual void runUpdateFuncs();
		
		virtual void uploadUniforms(const std::string& lightNum);

		virtual void uploadUniforms(Shader& shader, const std::string& lightNum) const;

		virtual GLuint updateUniformBlock(GLuint ubo, GLuint start);

		virtual void genShadowMap() = 0;

    private:
		std::map<std::string, std::function<void(Light*)>> updateFuncs;
};

class BasicLight : public Light
{
    public:
        GLuint VAO;
        glm::vec3 position;
		GLuint shadowFBO, depthMap;
		bool definedShadowMap = false;
		bool definedVAO = false;

		BasicLight(
			glm::vec3& ambient,
			glm::vec3& diffuse,
			glm::vec3& specular,
			glm::vec3& position,
			const Shader* shader,
			const std::string& prefix = std::string("b")
		);

		virtual glm::vec3 getPosition() const;

		virtual void setPosition(glm::vec3 position);

		virtual void addUpdateFunction(const std::string& name, std::function<void(BasicLight*)> lf);

		virtual void runUpdateFuncs();

		virtual void Draw();

		virtual void Draw(const Shader& shader) const;

		using Light::uploadUniforms;
		virtual void uploadUniforms(Shader& shader, const std::string& lightNum) const;

		virtual GLuint updateUniformBlock(GLuint ubo, GLuint start);

		virtual void genVAO();

		const bool hasVAO() const;

		virtual void genShadowMap();

		const bool hasShadowMap() const;

		virtual void drawShadowMap(std::function<void (const Shader& shader)> draw);

		virtual std::vector<glm::mat4> getShadowTransforms();

		virtual GLuint getDepthMap() const;


    private:
		size_t shadowWidth = 1024;
		size_t shadowHeight = 1024;
        GLuint VBO;
		std::map<std::string, std::function<void(BasicLight*)>> updateFuncs;
};


class PointLight : public BasicLight
{
    public:
        float constant;
        float linear;
        float quadratic;

		PointLight(
			glm::vec3& ambient,
			glm::vec3& diffuse,
			glm::vec3& specular,
			glm::vec3& position,
			float constant,
			float linear,
			float quadratic,
			const Shader* shader,
			const std::string& prefix = std::string("p")
		);

		const float getConstant() const;

		void setConstant(const float& constant);

		const float getLinear() const;

		void setLinear(const float& linear);

		const float getQuadratic() const;

		void setQuadratic(const float& quadratic);

		virtual void addUpdateFunction(const std::string& name, std::function<void(PointLight*)> lf);

		virtual void runUpdateFuncs();

		virtual void uploadUniforms(Shader& shader, const std::string& lightNum) const;

		virtual GLuint updateUniformBlock(GLuint ubo, GLuint start);

private:
		std::map<std::string, std::function<void(PointLight*)>> updateFuncs;
};

class DirectionLight : public BasicLight
{
    public:
        glm::vec3 direction;

		DirectionLight(
			glm::vec3& ambient,
			glm::vec3& diffuse,
			glm::vec3& specular,
			glm::vec3& position,
			glm::vec3& direction,
			const Shader* shader,
			const std::string& prefix = std::string("d")
		);

		virtual glm::vec3 getDirection() const;

		virtual void setDirection(glm::vec3 direction);

		virtual void addUpdateFunction(const std::string& name, std::function<void(DirectionLight*)> lf);

		virtual void runUpdateFuncs();

		virtual void uploadUniforms(Shader& shader, const std::string& lightNum) const;

		virtual GLuint updateUniformBlock(GLuint ubo, GLuint start);

		virtual void genShadowMap();

		virtual void drawShadowMap(std::function<void (const Shader& shader)> draw);

		virtual glm::mat4 getShadowTransform();

private:
		std::map<std::string, std::function<void(DirectionLight*)>> updateFuncs;
};

class SpotLight : public PointLight
{
    public:
        glm::vec3 direction;
        float cutOff;
        float outerCutOff;

		SpotLight(
			glm::vec3& ambient,
			glm::vec3& diffuse,
			glm::vec3& specular,
			glm::vec3& position,
			glm::vec3& direction,
			float& constant,
			float& linear,
			float& quadratic,
			float& cutOff,
			float& outerCutOff,
			const Shader* shader,
			const std::string& prefix = std::string("s")
		);

		virtual glm::vec3 getDirection() const;

		virtual void setDirection(glm::vec3 direction);

		virtual float getCutOff() const;

		virtual void setCutOff(float cutOff);

		virtual float getOuterCutOff() const;

		virtual void setOuterCutOff(float outerCutOff);

		virtual void addUpdateFunction(const std::string& name, std::function<void(SpotLight*)> lf);

		virtual void runUpdateFuncs();

		void uploadUniforms(Shader& shader, const std::string& lightNum) const;

		virtual GLuint updateUniformBlock(GLuint ubo, GLuint start);

private:
	std::map<std::string, std::function<void(SpotLight*)>> updateFuncs;
};

class LightManager
{
public:
	GLuint ubo;
	std::vector<BasicLight> basicLights;
	std::vector<PointLight> pointLights;
	std::vector<DirectionLight> directionLights;
	std::vector<SpotLight> spotLights;

	LightManager();

	void addBasicLight(BasicLight& blight);

	void addPointLight(PointLight& plight);

	void addDirectionLight(DirectionLight& dlight);

	void addSpotLight(SpotLight& slight);

	void drawLights();

	void uploadLightUniforms(Shader shader);

	void createUniformBlock();

	void updateUniformBlock();

	void runUpdateFuncs();

	void drawShadowMaps(std::function<void (const Shader& shader)> draw);
};
