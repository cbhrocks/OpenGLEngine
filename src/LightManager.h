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

#include "light.h"
#include "shader.h"
#include "glHelper.h"
#include "ShadowMap.h"
#include "ShadowCubeMap.h"

class LightManager
{
static const GLuint LIGHTS_UNIFORM_BLOCK_BINDING_POINT = 2;
public:
	LightManager();

	std::vector<PointLight*> getPointLights() { return this->pointLights; }
	void addPointLight(PointLight* plight) { this->pointLights.push_back(plight); }

	void addDirectionLight(DirectionLight* dlight) { this->directionLights.push_back(dlight); }
	std::vector<DirectionLight*> getDirectionLights() { return this->directionLights; }

	void addSpotLight(SpotLight* slight) { this->spotLights.push_back(slight); }
	std::vector<SpotLight*> getSpotLights() { return this->spotLights; }

	void addShadowMap(ShadowMap* shadowMap) { this->shadowMaps.push_back(shadowMap); }
	std::vector<ShadowMap*> getShadowMaps() { return this->shadowMaps; }

	void addShadowCubeMap(ShadowCubeMap* shadowCubeMap) { this->shadowCubeMaps.push_back(shadowCubeMap); }
	std::vector<ShadowCubeMap*> getShadowCubeMaps() { return this->shadowCubeMaps; }


	void drawLights(const Shader& shader);

	void createUniformBlock();

	void updateUniformBlock();

private:
	GLuint ubo;
	GLuint VAO = 0, VBO;
	std::vector<PointLight*> pointLights;
	std::vector<DirectionLight*> directionLights;
	std::vector<SpotLight*> spotLights;
	std::vector<ShadowMap*> shadowMaps;
	std::vector<ShadowCubeMap*> shadowCubeMaps;
};
