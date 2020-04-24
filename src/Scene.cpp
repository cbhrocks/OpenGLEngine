#include "Scene.h"

Scene::Scene() :
	up(0.0, 1.0, 0.0),
	currentTime(0.0),
	gammaCorrection(2.2f),
	exposure(1.0f),
	activeCamera(0),
	bloom(true)
{
	this->setup();
	this->initializeShaders();
}

LightManager* Scene::getLightManager() const {
	return this->lightManager;
}

void Scene::setLightManager(LightManager* lightManager) {
	this->lightManager = lightManager;
}

void Scene::setModel(std::string name, Model* model) {
	this->models[name] = model;
	//this->models.push_back(model);
}

Model* Scene::getModel(std::string name) {
	return this->models[name];
}

Model* Scene::removeModel(std::string name) {
	Model* model = this->models.at(name);
	this->models.erase(name);
	return model;
}

void Scene::addObject(DrawObject* object) {
	this->drawObjects.push_back(object);
}

void Scene::clearObjects() {
	//this->lightManager = new LightManager();
	this->models.clear();
	this->drawObjects.clear();
	//this->cameras.clear();
}

void Scene::onFrame()
{
	this->lightManager->runUpdateFuncs();
	this->lightManager->updateUniformBlock();
	this->getActiveCamera()->updateUniformBlock();
}

void Scene::uploadSkyboxUniforms(const Shader& shader) {
	shader.Use();
	glm::mat4 projection = this->getActiveCamera()->getProjectionMatrix();
	glm::mat4 view = glm::mat4(glm::mat3(this->getActiveCamera()->getViewMatrix()));

	shader.setMat4("P", projection);
	shader.setMat4("V", view);

	checkGLError("upload skybox uniforms");
}

void Scene::draw() {
	this->drawShadows();
	Camera* camera = this->getActiveCamera();
	if (camera->getTBM() != nullptr) {
		this->getActiveCamera()->getTBM()->setActive();
	}
	//this->lightManager->bindShadowTextures(0);
	this->drawModels();
	if (camera->getTBM() != nullptr) {
		this->getActiveCamera()->getTBM()->Draw();
	}
}

void Scene::drawShadows() {
	this->lightManager->drawShadowMaps([this](const Shader& shader) {
		this->drawModels(shader);
	});

	glBindFramebuffer(GL_FRAMEBUFFER, 0);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	//glViewport(0, 0, this->width, this->height);
}

void Scene::drawSkybox() {
	uploadSkyboxUniforms(skybox->getShader());
	skybox->uploadUniforms();
	skybox->Draw();
}

void Scene::drawModels(const Shader& shader) {
	for (std::map<std::string, Model*>::iterator it = this->models.begin(); it != this->models.end(); ++it) {
		it->second->uploadUniforms(shader);
		it->second->Draw(shader);
	}
	//this->lightManager->drawLights();

	std::map<float, DrawObject*> sorted;
	//draw non transparent models
	for (int i = 0; i < this->drawObjects.size(); i++) {
		if (!this->drawObjects.at(i)->isTransparent()) {
			this->drawObjects.at(i)->UploadUniforms(shader);
			this->drawObjects.at(i)->Draw(shader);
		}
		//add transparent models to map with key distance since maps are sorted by key by default.
		else {
			float distance = glm::length(this->getActiveCamera()->getPosition() - this->drawObjects.at(i)->getPosition());
			sorted[distance] = this->drawObjects.at(i);
		}
	}
	//draw transparent models
	for (std::map<float, DrawObject*>::reverse_iterator it = sorted.rbegin(); it != sorted.rend(); ++it)
	{
		it->second->UploadUniforms(shader);
		it->second->Draw(shader);
	}
}

void Scene::drawModels()
{
	for (std::map<std::string, Model*>::iterator it = this->models.begin(); it != this->models.end(); ++it) {
		it->second->uploadUniforms();
		it->second->Draw();
	}

	this->lightManager->drawLights();

	std::map<float, DrawObject*> sorted;
	//draw non transparent models
	for (int i = 0; i < this->drawObjects.size(); i++) {
		if (!this->drawObjects.at(i)->isTransparent()) {
			this->drawObjects.at(i)->UploadUniforms();
			this->drawObjects.at(i)->Draw();
		}
		//add transparent models to map with key distance since maps are sorted by key by default.
		else {
			float distance = glm::length(this->getActiveCamera()->getPosition() - this->drawObjects.at(i)->getPosition());
			sorted[distance] = this->drawObjects.at(i);
		}
	}
	//draw transparent models
	//TODO: look into putting the highlight drawing elsewhere, or make it independent of the drawobj's shader
	for (std::map<float, DrawObject*>::reverse_iterator it = sorted.rbegin(); it != sorted.rend(); ++it)
	{
		//it->second->getShader()->Use();
		it->second->UploadUniforms();
		it->second->Draw();
	}
}

void Scene::timeStep(double t)
{
	this->currentTime = t;
	this->updateUbo();
}

Camera* Scene::getActiveCamera() const
{
	return this->cameras.at(this->activeCamera);
}

void Scene::setActiveCamera(const int &activeCamera)
{
	this->activeCamera = activeCamera;
}

void Scene::addCamera(Camera* camera)
{
	this->cameras.push_back(camera);
}

glm::vec3 Scene::getUp() const
{
	return this->up;
}

void Scene::setUp(glm::vec3 up)
{
	this->up = up;
}

void Scene::setGammaCorrection(float gamma)
{
	this->gammaCorrection = gamma;
	printf("Gamma correction set to: %f", gamma);
	this->clearObjects();
}

float Scene::getGammaCorrection() const
{
	return this->gammaCorrection;
}

void Scene::setExposure(float exposure)
{
	 this->exposure = exposure;
}

float Scene::getExposure() const
{
	return this->exposure;
}

void Scene::setBloom(bool bloom)
{
	this->bloom = bloom;
}

bool Scene::getBloom() const
{
	return this->bloom;
}

void Scene::initializeShaders()
{
	checkGLError("Scene::initializeShaders -- Scene shaders");
}

void Scene::setup() {
	this->setupUbo();
}

void Scene::setupUbo() {
	int uboSize = 4// time
		+ 4 //gamma
		+ 4 //exposure
		+ 4; //bloom
	glGenBuffers(1, &this->ubo);
	glBindBuffer(GL_UNIFORM_BUFFER, this->ubo);
	glBufferData(GL_UNIFORM_BUFFER, uboSize, NULL, GL_STATIC_DRAW);
	glBindBuffer(GL_UNIFORM_BUFFER, 0);
	glBindBufferRange(GL_UNIFORM_BUFFER, 0, this->ubo, 0, uboSize);
	checkGLError("Scene::setupUbo");
}

void Scene::updateUbo() {
	float time = this->currentTime;
	float gamma = this->gammaCorrection;
	float exposure = this->exposure;
	bool bloom = this->bloom;
	glBindBuffer(GL_UNIFORM_BUFFER, this->ubo);
	glBufferSubData(GL_UNIFORM_BUFFER, 0, 4, &time);
	glBufferSubData(GL_UNIFORM_BUFFER, 4, 4, &gamma);
	glBufferSubData(GL_UNIFORM_BUFFER, 8, 4, &exposure);
	glBufferSubData(GL_UNIFORM_BUFFER, 12, 4, &bloom);
	glBindBuffer(GL_UNIFORM_BUFFER, 0);
	checkGLError("Scene::updateUbo");
}
