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

	Scene();

	std::map<std::string, Model*> getModels();
	void setModels(std::map<std::string, Model*> models);

	std::vector<std::string> getTransparentModels();
	std::vector<std::string> getOpaqueModels();

	void setModel(std::string name, Model* model);
	Model* getModel(std::string);
	Model* removeModel(std::string);

	LightManager* getLightManager() const;
	void setLightManager(LightManager* lightManager);

	Camera* getActiveCamera() const;
	void setActiveCamera(const int &activeCamera);
	void addCamera(Camera* camera);

	void setSkybox(Skybox* skybox);
	Skybox* getSkybox();

	glm::vec3 getUp() const;
	void setUp(glm::vec3 up);

private:
	Skybox* skybox;
	LightManager* lightManager;
	//std::vector<Model*> models;
	std::map<std::string, Model*> models;
	std::vector<std::string> transparentModels;
	std::vector<std::string> opaqueModels;
	std::vector<DrawObject*> drawObjects;
	std::vector<Camera*> cameras;
	int activeCamera;
};
