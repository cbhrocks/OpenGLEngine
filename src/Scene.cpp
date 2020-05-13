#include "Scene.h"

Scene::Scene() :
	up(0.0, 1.0, 0.0),
	activeCamera(0)
{ }

LightManager* Scene::getLightManager() const { return this->lightManager; }
void Scene::setLightManager(LightManager* lightManager) { this->lightManager = lightManager; }

std::map<std::string, Model*> Scene::getModels() { return this->models; }
void Scene::setModels(std::map<std::string, Model*> models) { 
	this->models = models; 
	for (auto it : models) {
		if (it.second->getTransparent()) {
			this->transparentModels.push_back(it.first);
		}
		else {
			this->opaqueModels.push_back(it.first);
		}
	}
}

std::vector<std::string> Scene::getTransparentModels() { return this->transparentModels; }
std::vector<std::string> Scene::getOpaqueModels() { return this->opaqueModels; }

Model* Scene::getModel(std::string name) { return this->models[name]; }
void Scene::setModel(std::string name, Model* model) { 
	this->models[name] = model; 
	if (model->getTransparent()) {
		this->transparentModels.push_back(name);
	}
	else {
		this->opaqueModels.push_back(name);
	}
}
Model* Scene::removeModel(std::string name) {
	Model* model = this->models.at(name);
	this->models.erase(name);
	return model;
}

Camera* Scene::getActiveCamera() const { return this->cameras.at(this->activeCamera); }
void Scene::setActiveCamera(const int &activeCamera) { this->activeCamera = activeCamera; }
void Scene::addCamera(Camera* camera) { this->cameras.push_back(camera); }

Skybox* Scene::getSkybox() { return this->skybox; }
void Scene::setSkybox(Skybox* skybox) { this->skybox = skybox; }

glm::vec3 Scene::getUp() const { return this->up; }
void Scene::setUp(glm::vec3 up) { this->up = up; }
