#pragma once

#include <glad/glad.h>
#include <iostream>
#include "model.h"
#include "light.h"
#include "shader.h"
#include "skybox.h"
#include "DrawObj.h"
#include "TextureManager.h"
#include "FBOManager.h"
#include "camera.h"

class Scene {
public:
	glm::vec3 up;
	std::map<std::string, const Shader> shaders;
	double currentTime;
	float gammaCorrection;
	float exposure;
	bool bloom;

	Scene();

	std::map<std::string, Model*> getModels();
	void setModels(std::map<std::string, Model*> models);

	void setModel(std::string name, Model* model);
	Model* getModel(std::string);
	Model* removeModel(std::string);

	void addObject(DrawObject* object);

	void clearObjects();

	void onFrame();

	void uploadSkyboxUniforms(const Shader& shader);

	void draw();
	void drawShadows();
	void drawSkybox();
	void drawModels(const Shader& shader);
	void drawModels();

	void timeStep(double t);

	Camera* getActiveCamera() const;
	void setActiveCamera(const int &activeCamera);
	void addCamera(Camera* camera);

	glm::vec3 getUp() const;
	void setUp(glm::vec3 up);

	LightManager* getLightManager() const;
	void setLightManager(LightManager* lightManager);

	void setGammaCorrection(float gamma);
	float getGammaCorrection() const;

	void setExposure(float exposure);
	float getExposure() const;

	void setBloom(bool bloom);
	bool getBloom() const;

	void initializeShaders();

private:
	GLuint ubo;
	Skybox* skybox;
	LightManager* lightManager;
	//std::vector<Model*> models;
	std::map<std::string, Model*> models;
	std::vector<DrawObject*> drawObjects;
	std::vector<Camera*> cameras;
	int activeCamera;

	void setup();

	void setupUbo();
	void updateUbo();
};
