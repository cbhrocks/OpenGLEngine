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

	//glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

	glEnable(GL_PROGRAM_POINT_SIZE);
	checkGLError("Renderer::initialize");

	this->setupUbo();

	this->shaders = {
		//debug
		{"vertexNormalLines", Shader("src/shaders/vertexNormalLines.vert", "src/shaders/vertexNormalLines.frag", "src/shaders/vertexNormalLines.geom").setUniformBlock("Camera", 1) },
		{"faceNormalLines", Shader("src/shaders/faceNormalLines.vert", "src/shaders/faceNormalLines.frag", "src/shaders/faceNormalLines.geom").setUniformBlock("Camera", 1) },
		{"tbnLines", Shader("src/shaders/tbnLines.vert", "src/shaders/tbnLines.frag", "src/shaders/tbnLines.geom").setUniformBlock("Camera", 1)},
		{"depth", Shader("src/shaders/depth.vert", "src/shaders/depth.frag").setUniformBlock("Camera", 1)},
		// post processing
		{"shader2D", Shader("src/shaders/basic2D.vert", "src/shaders/basic2D.frag").setUniformBlock("Scene", 0)},
		{"gBlur2D", Shader("src/shaders/gaussianBlur2D.vert", "src/shaders/gaussianBlur2D.frag").setUniformBlock("Scene", 0)},
		{"hdr2D", Shader("src/shaders/basic2D.vert", "src/shaders/hdr2D.frag").setUniformBlock("Scene", 0)},
		{"inverse2D", Shader("src/shaders/basic2D.vert", "src/shaders/inverse2D.frag").setUniformBlock("Scene", 0)},
		{"grey2D", Shader("src/shaders/basic2D.vert", "src/shaders/grey2D.frag").setUniformBlock("Scene", 0)},
		{"sharpen2D", Shader("src/shaders/basic2D.vert", "src/shaders/sharpen2D.frag").setUniformBlock("Scene", 0)},
		{"blur2D", Shader("src/shaders/basic2D.vert", "src/shaders/blur2D.frag").setUniformBlock("Scene", 0)},
		{"edge2D", Shader("src/shaders/basic2D.vert", "src/shaders/edge2D.frag").setUniformBlock("Scene", 0)},
		{"bloom2D", Shader("src/shaders/bloom2D.vert", "src/shaders/bloom2D.frag").setUniformBlock("Scene", 0)},
		//gbuffer
		{"gBufferGeometry", Shader("src/shaders/gBuffer.vert", "src/shaders/gBuffer.frag").setUniformBlock("Camera", 1)},
		{"gBufferDeferred", Shader("src/shaders/deferred_shading.vert", "src/shaders/deferred_shading.frag").setUniformBlock("Camera", 1).setUniformBlock("Lights", 2)},
		// drawing
		{"basic", Shader("src/shaders/basic.vert", "src/shaders/basic.frag").setUniformBlock("Camera", 1)},
		{"texture", Shader("src/shaders/basic.vert", "src/shaders/texture.frag").setUniformBlock("Camera", 1)},
		{"trans", Shader("src/shaders/basic.vert", "src/shaders/trans.frag").setUniformBlock("Camera", 1)},
		{"skybox", Shader("src/shaders/skybox.vert", "src/shaders/skybox.frag")},
		{"highlight", Shader("src/shaders/basic.vert", "src/shaders/highlight.frag").setUniformBlock("Camera", 1)},
		{"reflection", Shader("src/shaders/reflection.vert", "src/shaders/reflection.frag").setUniformBlock("Camera", 1)},
		{"explode", Shader("src/shaders/explode.vert", "src/shaders/texture.frag", "src/shaders/explode.geom").setUniformBlock("Scene", 0).setUniformBlock("Camera", 1)},
		{"light", Shader("src/shaders/basic.vert", "src/shaders/light.frag").setUniformBlock("Camera", 1)},
		// lighting
		{"shadowDepth", Shader("src/shaders/shadowDepth.vert", "src/shaders/shadowDepth.frag")},
		{"shadowCubeDepth", Shader("src/shaders/shadowDepthCube.vert", "src/shaders/shadowDepthCube.frag", "src/shaders/shadowDepthCube.geom")},
		{"shadowDebug", Shader("src/shaders/shadowDebug.vert", "src/shaders/shadowDebug.frag").setUniformBlock("Camera", 1)},
		{"phongLighting", Shader("src/shaders/lighting.vert", "src/shaders/phongLighting.frag").setUniformBlock("Camera", 1).setUniformBlock("Lights", 2)},
		{"blinnPhongLighting", Shader("src/shaders/lighting.vert", "src/shaders/blinnPhongLighting.frag").setUniformBlock("Scene", 0).setUniformBlock("Camera", 1).setUniformBlock("Lights", 2)},
		{"BPLightingNorm", Shader("src/shaders/BPLightingNorm.vert", "src/shaders/BPLightingNorm.frag").setUniformBlock("Scene", 0).setUniformBlock("Camera", 1).setUniformBlock("Lights", 2)}
	};
	checkGLError("Renderer::initialize -- shaders");
	this->shaders["gBufferDeferred"].Use();
	this->shaders["gBufferDeferred"].setInt("gPosition", 0).setInt("gNormal", 1).setInt("gAlbedoSpec", 2);
	checkGLError("Renderer::initialize -- shaders");
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
	// render shadows
	//if (this->shadowsEnabled) {
	//	this->renderShadows(scene);
	//}
	// render all the models without forward rendering enabled
	auto models = scene->getModels();

	this->gBuffer->setActive();
	for (auto &it : models) {
		if (this->forwardRenderModels.count(it.first) < 1) {
			it.second->uploadUniforms(this->shaders["gBufferGeometry"]);
			it.second->Draw(this->shaders["gBufferGeometry"]);
		}
	}
	this->gBuffer->Draw(this->shaders["gBufferDeferred"]);
	this->gBuffer->copyDepth(0, this->width, this->height);
	// render the models with forward rendering
	for (auto &it : this->forwardRenderModels) {
		Model* model = models.at(it.first);
		const Shader& shader = this->shaders.at(it.second);
		model->uploadUniforms(shader);
		model->Draw(shader);
	}
	// render lights for debug purposes
	scene->getLightManager()->drawLights(this->shaders["light"]);
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
	//scene->getLightManager()->drawLights();
}

void Renderer::renderSkybox(Scene* scene)
{
	Skybox* skybox = scene->getSkybox();
	skybox->uploadUniforms();
	skybox->Draw();
}

// deferred rendering

void Renderer::renderToGBuffer(Scene* scene) {
	this->gBuffer->setActive();
	for (auto &it : scene->getModels()) {
		it.second->uploadUniforms(this->shaders["gBufferGeometry"]);
		it.second->Draw(this->shaders["gBufferGeometry"]);
	}
	this->gBuffer->Draw(this->shaders["gBufferDeferred"]);
}

void Renderer::renderVertexNormalLines(Scene* scene) { 
	const Shader& shader = this->shaders.at("vertexNormalLines");
	for (auto &it : scene->getModels()) {
		it.second->uploadUniforms(shader);
		it.second->Draw(shader);
	}
}
void Renderer::renderTBNLines(Scene* scene) { 
	const Shader& shader = this->shaders.at("tbnLines");
	for (auto &it : scene->getModels()) {
		it.second->uploadUniforms(shader);
		it.second->Draw(shader);
	}
}
void Renderer::renderVertexFaceLines(Scene* scene) { 
	const Shader& shader = this->shaders.at("vertexFaceLines");
	for (auto &it : scene->getModels()) {
		it.second->uploadUniforms(shader);
		it.second->Draw(shader);
	}
}
void Renderer::renderDepth(Scene* scene) { 
	const Shader& shader = this->shaders.at("depth");
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

GBuffer* Renderer::getGBuffer() const { return this->gBuffer; }
void Renderer::setGBuffer(GBuffer* gBuffer) { this->gBuffer = gBuffer; }

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
