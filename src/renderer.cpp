#include "renderer.h"

Renderer::Renderer(int width, int height) :
	width(width),
	height(height),
	time(0.0),
	gammaCorrection(2.2f),
	exposure(1.0f),
	bloom(true)
{
	glEnable(GL_CULL_FACE);
	glCullFace(GL_BACK);

	glEnable(GL_DEPTH_TEST);
	glDepthFunc(GL_LESS);

	//glEnable(GL_STENCIL_TEST);
	//glStencilOp(GL_KEEP, GL_KEEP, GL_REPLACE);
	//glStencilFunc(GL_ALWAYS, 1, 0xFF);
	//glStencilOpSeparate(GL_BACK, GL_KEEP, GL_KEEP, GL_KEEP);
	//glStencilOpSeparate(GL_FRONT, GL_KEEP, GL_REPLACE, GL_REPLACE);

	//glEnable(GL_FRAMEBUFFER_SRGB);

	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

	glEnable(GL_PROGRAM_POINT_SIZE);
	checkGLError("Renderer::initialize");

	this->setupUbo();

	this->debugShaders = {
		{"vertexNormalLines", Shader("src/shaders/vertexNormalLines.vert", "src/shaders/vertexNormalLines.frag", "src/shaders/vertexNormalLines.geom").setUniformBlock("Camera", 1) },
		{"faceNormalLines", Shader("src/shaders/faceNormalLines.vert", "src/shaders/faceNormalLines.frag", "src/shaders/faceNormalLines.geom").setUniformBlock("Camera", 1) },
		{"tbnLines", Shader("src/shaders/tbnLines.vert", "src/shaders/tbnLines.frag", "src/shaders/tbnLines.geom").setUniformBlock("Camera", 1)},
		{"depth", Shader("src/shaders/depth.vert", "src/shaders/depth.frag").setUniformBlock("Camera", 1)}
	};
}

// render methods

void Renderer::preRender(Scene* scene)
{
	glClearColor(0.1f, 0.1f, 0.1f, 1.0f);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);

	// run any functions to update position based on time
	scene->getLightManager()->runUpdateFuncs();

	// update uniform block objects for use during shaders
	this->updateUbo();
	scene->getLightManager()->updateUniformBlock();
	scene->getActiveCamera()->updateUniformBlock();
}

void Renderer::postRender(Scene* scene)
{

}

void Renderer::render(Scene* scene)
{
	if (this->shadowsEnabled) {
		this->renderShadows(scene);
	}
	if (this->tbm != nullptr && this->useFBO) {
		//this->tbm->setActive();
	}
	this->renderModels(scene);
	if (this->tbm != nullptr && this->useFBO) {
		//this->tbm->Draw();
	}
	// render lights for debug purposes
	this->renderLights(scene);
}

void Renderer::renderShadows(Scene* scene)
{
	scene->getLightManager()->drawShadowMaps([&, scene](const Shader& shader) {
		std::map<std::string, Model*> models = scene->getModels();
		for (auto &it = models.begin(); it != models.end(); ++it) {
			it->second->uploadUniforms(shader);
			it->second->Draw(shader);
		}
	});

	glBindFramebuffer(GL_FRAMEBUFFER, 0);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);
}

void Renderer::renderModels(Scene* scene)
{
	std::map<std::string, Model*> models = scene->getModels();

	std::map<float, Model*> sortedTransModels;
	// draw all opaque models, remove translucent to sorted map
	for (auto &it : models) {
		if (it.second->getTransparent()) {
			float distance = glm::length(scene->getActiveCamera()->getPosition() - it.second->getPosition());
			sortedTransModels.insert(std::make_pair(distance, it.second));
		}
		else {
			it.second->uploadUniforms();
			it.second->Draw();
		}
	}
	// draw all translucent models from back to front
	for (auto &it = sortedTransModels.rbegin(); it != sortedTransModels.rend(); ++it) {
		it->second->uploadUniforms();
		it->second->Draw();
	}
}

void Renderer::renderLights(Scene* scene)
{
	scene->getLightManager()->drawLights();
}

void Renderer::renderSkybox(Scene* scene)
{
	Skybox* skybox = scene->getSkybox();
	skybox->uploadUniforms();
	skybox->Draw();
}

void Renderer::renderVertexNormalLines(Scene* scene) { 
	Shader& shader = debugShaders.at("vertexNormalLines");
	for (auto &it : scene->getModels()) {
		it.second->uploadUniforms(shader);
		it.second->Draw(shader);
	}
}
void Renderer::renderTBNLines(Scene* scene) { 
	Shader& shader = debugShaders.at("tbnLines");
	for (auto &it : scene->getModels()) {
		it.second->uploadUniforms(shader);
		it.second->Draw(shader);
	}
}
void Renderer::renderVertexFaceLines(Scene* scene) { 
	Shader& shader = debugShaders.at("vertexFaceLines");
	for (auto &it : scene->getModels()) {
		it.second->uploadUniforms(shader);
		it.second->Draw(shader);
	}
}
void Renderer::renderDepth(Scene* scene) { 
	Shader& shader = debugShaders.at("depth");
	for (auto &it : scene->getModels()) {
		it.second->uploadUniforms(shader);
		it.second->Draw(shader);
	}
}

void Renderer::toggleWireframeMode()
{
	GLint polyMode;
	glGetIntegerv(GL_POLYGON_MODE, &polyMode);

	if (polyMode == GL_LINE) {
		glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
		this->useFBO = true;
	}
	if (polyMode == GL_FILL) {
		glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
		this->useFBO = false;
	}
	if (polyMode == GL_POINTS)
		;//
}

// getter and setters

FBOManagerI* Renderer::getTBM() const { return this->tbm; }
void Renderer::setTBM(FBOManagerI* tbm) { this->tbm = tbm; }

void Renderer::setTime(float time) { this->time = time; }
float Renderer::getTime() const { return this->time; }

void Renderer::setGammaCorrection(bool gamma) { this->gammaCorrection = gamma; }
bool Renderer::getGammaCorrection() const { return this->gammaCorrection; }

void Renderer::setExposure(float exposure) { this->exposure = exposure; }
float Renderer::getExposure() const { return this->exposure; }

void Renderer::setBloom(bool bloom) { this->bloom = bloom; }
bool Renderer::getBloom() const { return this->bloom; }

void Renderer::setRes(int width, int height) {
	glViewport(0, 0, this->width, this->height);
	this->tbm->setDimensions(width, height);
}

void Renderer::updateUbo() {
	glBindBuffer(GL_UNIFORM_BUFFER, this->ubo);
	glBufferSubData(GL_UNIFORM_BUFFER, 0, 4, &this->time);
	glBufferSubData(GL_UNIFORM_BUFFER, 4, 4, &this->gammaCorrection);
	glBufferSubData(GL_UNIFORM_BUFFER, 8, 4, &this->exposure);
	glBufferSubData(GL_UNIFORM_BUFFER, 12, 4, &this->bloom);
	glBindBuffer(GL_UNIFORM_BUFFER, 0);
	checkGLError("Scene::updateUbo");
}

void Renderer::setupUbo() {
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
