#include "renderer.h"

Renderer::Renderer(int width, int height) :
	width(width),
	height(height),
	nearBound(0.1f),
	farBound(90.0f),
	fieldOfView(90),
	drawLights(true),
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

	glViewport(0, 0, width, height);

	checkGLError("Renderer::initialize");

	this->setupUbo();

	this->shaders = {
		//debug
		{"vertexNormalLines", Shader("src/shaders/vertexNormalLines.vert", "src/shaders/vertexNormalLines.frag", "src/shaders/vertexNormalLines.geom").setUniformBlock("Scene", 0).setUniformBlock("Camera", 1) },
		{"faceNormalLines", Shader("src/shaders/faceNormalLines.vert", "src/shaders/faceNormalLines.frag", "src/shaders/faceNormalLines.geom").setUniformBlock("Scene", 0).setUniformBlock("Camera", 1) },
		{"tbnLines", Shader("src/shaders/tbnLines.vert", "src/shaders/tbnLines.frag", "src/shaders/tbnLines.geom").setUniformBlock("Scene", 0).setUniformBlock("Camera", 1)},
		{"linearDepth", Shader("src/shaders/linear_depth.vert", "src/shaders/linear_depth.frag").setUniformBlock("Scene", 0).setUniformBlock("Camera", 1)},
		{"debugTextureQuad", Shader("src/shaders/debugTextureQuad.vert", "src/shaders/debugTextureQuad.frag").Use().setInt("textureUnit", 0)},
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
		{"gBufferGeometry", Shader("src/shaders/gBuffer.vert", "src/shaders/gBuffer.frag").setUniformBlock("Scene", 0).setUniformBlock("Camera", 1)},
		{"gBufferDLight", Shader("src/shaders/ds_dlight_pass.vert", "src/shaders/ds_dlight_pass.frag").setUniformBlock("Camera", 1).setUniformBlock("Lights", 2).Use().setInt("gPosition", 0).setInt("gNormal", 1).setInt("gAlbedoSpec", 2)},
		{"gBufferPLight", Shader("src/shaders/ds_plight_pass.vert", "src/shaders/ds_plight_pass.frag").setUniformBlock("Scene", 0).setUniformBlock("Camera", 1).Use().setInt("gPosition", 0).setInt("gNormal", 1).setInt("gAlbedoSpec", 2)},
		{"depth", Shader("src/shaders/depth.vert", "src/shaders/depth.frag").setUniformBlock("Scene", 0).setUniformBlock("Camera", 1)},
		// drawing
		{"basic", Shader("src/shaders/basic.vert", "src/shaders/basic.frag").setUniformBlock("Scene", 0).setUniformBlock("Camera", 1)},
		{"texture", Shader("src/shaders/basic.vert", "src/shaders/texture.frag").setUniformBlock("Scene", 0).setUniformBlock("Camera", 1)},
		{"trans", Shader("src/shaders/basic.vert", "src/shaders/trans.frag").setUniformBlock("Scene", 0).setUniformBlock("Camera", 1)},
		{"skybox", Shader("src/shaders/skybox.vert", "src/shaders/skybox.frag")},
		{"highlight", Shader("src/shaders/basic.vert", "src/shaders/highlight.frag").setUniformBlock("Scene", 0).setUniformBlock("Camera", 1)},
		{"reflection", Shader("src/shaders/reflection.vert", "src/shaders/reflection.frag").setUniformBlock("Scene", 0).setUniformBlock("Camera", 1)},
		{"explode", Shader("src/shaders/explode.vert", "src/shaders/texture.frag", "src/shaders/explode.geom").setUniformBlock("Scene", 0).setUniformBlock("Camera", 1)},
		{"light", Shader("src/shaders/basic.vert", "src/shaders/light.frag").setUniformBlock("Scene", 0).setUniformBlock("Camera", 1)},
		// lighting
		{"material", Shader("src/shaders/material.vert", "src/shaders/material.frag").setUniformBlock("Scene", 0).setUniformBlock("Camera", 1).setUniformBlock("Lights", 2)},
		{"shadowDepth", Shader("src/shaders/shadowDepth.vert", "src/shaders/shadowDepth.frag")},
		{"shadowCubeDepth", Shader("src/shaders/shadowDepthCube.vert", "src/shaders/shadowDepthCube.frag", "src/shaders/shadowDepthCube.geom")},
		{"shadowDebug2D", Shader("src/shaders/shadowDebug.vert", "src/shaders/shadowDebug.frag").setUniformBlock("Camera", 1).Use().setInt("depthMap", 0)},
		{"shadowCubeDebug", Shader("src/shaders/shadowCubeDebug.vert", "src/shaders/shadowCubeDebug.frag").setUniformBlock("Camera", 1).Use().setInt("depthMap", 1)},
		{"phongLighting", Shader("src/shaders/lighting.vert", "src/shaders/phongLighting.frag").setUniformBlock("Scene", 0).setUniformBlock("Camera", 1).setUniformBlock("Lights", 2)},
		{"directionalShadows", Shader("src/shaders/directionalShadows.vert", "src/shaders/directionalShadows.frag").setUniformBlock("Scene", 0).setUniformBlock("Camera", 1).setUniformBlock("Lights", 2).Use().setInt("shadowMap", 0)},
		{"pointShadows", Shader("src/shaders/pointShadows.vert", "src/shaders/pointShadows.frag").setUniformBlock("Scene", 0).setUniformBlock("Camera", 1).setUniformBlock("Lights", 2).Use().setInt("shadowCubeMap", 1)},
		{"blinnPhongLighting", Shader("src/shaders/lighting.vert", "src/shaders/blinnPhongLighting.frag").setUniformBlock("Scene", 0).setUniformBlock("Camera", 1).setUniformBlock("Lights", 2)},
		{"BPLightingNorm", Shader("src/shaders/BPLightingNorm.vert", "src/shaders/BPLightingNorm.frag").setUniformBlock("Scene", 0).setUniformBlock("Camera", 1).setUniformBlock("Lights", 2)}
	};
	int uniformBlockSize = this->shaders["BPLightingNorm"].getUniformBlockSize("Scene");
	int window_size_offset = this->shaders["BPLightingNorm"].getUniformOffset({ 
		"projection",
		"window_size",
		"time",
		"gamma",
		"exposure",
		"bloom",
	});
	uniformBlockSize = this->shaders["BPLightingNorm"].getUniformBlockSize("Camera");
	window_size_offset = this->shaders["BPLightingNorm"].getUniformOffset({ 
		"view",
		"camPos",
	});
	checkGLError("Renderer::initialize -- shaders");
}

// render methods

void Renderer::preRender(Scene* scene)
{
	glClearColor(1.0f, 1.0f, 1.0f, 1.0f);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	// update uniform block objects for use during shaders
	this->updateUbo();
	scene->getLightManager()->updateUniformBlock();
	scene->getActiveCamera()->updateUniformBlock();
}

void Renderer::render(Scene* scene)
{
	// render shadow depth maps
	if (this->renderShadows) {
		this->renderShadowMaps(scene);
	}

	// render all the models without forward rendering enabled
	auto models = scene->getModels();

	this->gBuffer->BindForWriting();
	for (auto &it : models) {
		if (this->modelShaders.count(it.first) < 1) {
			it.second->uploadUniforms(this->shaders["gBufferGeometry"]);
			it.second->Draw(this->shaders["gBufferGeometry"]);
		}
	}

	this->gBuffer->DSLightingPass(
		this->shaders["gBufferDLight"], 
		this->shaders["gBufferPLight"], 
		this->shaders["depth"], 
		scene->getLightManager()->getPointLights()
	);

	glEnable(GL_DEPTH_TEST);
	glDepthMask(GL_TRUE);

	// disable blending that was used to add direction and point lighting to the scene
	glDisable(GL_BLEND);

	this->gBuffer->copyDepth(0, this->width, this->height);

	// render the models with forward rendering
	for (auto &it : this->modelShaders) {
		Model* model = models.at(it.first);
		const Shader& shader = this->shaders.at(it.second);

		// upload shadow uniforms and bind shadow textures
		std::vector<ShadowMap*> shadowMaps = scene->getLightManager()->getShadowMaps();
		int textureNum = 0;
		for (int i = 0; i < shadowMaps.size(); i++) {
			shadowMaps[i]->uploadUniforms(shader);
			glActiveTexture(GL_TEXTURE0 + textureNum++);
			glBindTexture(GL_TEXTURE_2D, shadowMaps[i]->getTexture());
		}

		std::vector<ShadowCubeMap*> shadowCubeMaps = scene->getLightManager()->getShadowCubeMaps();
		for (int i = 0; i < shadowCubeMaps.size(); i++) {
			shadowCubeMaps[i]->uploadUniforms(shader);
			glActiveTexture(GL_TEXTURE0 + textureNum++);
			glBindTexture(GL_TEXTURE_CUBE_MAP, shadowCubeMaps[i]->getTexture());
		}

		model->uploadUniforms(shader);
		model->Draw(shader, textureNum);
	}

	// render lights for debug purposes
	if (this->drawLights)
		this->renderLights(scene);
}

void Renderer::renderShadowMaps(Scene* scene)
{
	std::map<std::string, Model*> models = scene->getModels();
	std::vector<ShadowMap*> shadowMaps = scene->getLightManager()->getShadowMaps();
	std::vector<ShadowCubeMap*> shadowCubeMaps = scene->getLightManager()->getShadowCubeMaps();
	for (auto shadowMap : shadowMaps) {
		shadowMap->setActive();
		shadowMap->uploadUniforms(this->shaders["shadowDepth"]);
		for (auto model_it : models) {
			model_it.second->uploadUniforms(this->shaders["shadowDepth"]);
			model_it.second->Draw(this->shaders["shadowDepth"]);
		}
	}
	for (auto shadowCubeMap : shadowCubeMaps) {
		shadowCubeMap->setActive();
		shadowCubeMap->uploadUniforms(this->shaders["shadowCubeDepth"]);
		for (auto model_it : models) {
			model_it.second->uploadUniforms(this->shaders["shadowCubeDepth"]);
			model_it.second->Draw(this->shaders["shadowCubeDepth"]);
		}
	}

	glViewport(0, 0, this->width, this->height);
	glBindFramebuffer(GL_FRAMEBUFFER, 0);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
}

void Renderer::renderLights(Scene* scene)
{
	scene->getLightManager()->drawLights(this->shaders["light"]);
}

void Renderer::renderSkybox(Scene* scene)
{
	Skybox* skybox = scene->getSkybox();
	skybox->uploadUniforms();
	skybox->Draw();
}

// deferred rendering

void Renderer::renderToGBuffer(Scene* scene) {
	this->gBuffer->BindForWriting();
	for (auto &it : scene->getModels()) {
		it.second->uploadUniforms(this->shaders["gBufferGeometry"]);
		it.second->Draw(this->shaders["gBufferGeometry"]);
	}
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

void Renderer::setDimensions(int width, int height) {
	this->width = width;
	this->height = height;
	glViewport(0, 0, this->width, this->height);
	if (this->gBuffer) {
		this->gBuffer->setDimensions(width, height);
	}
	//this->tbm->setDimensions(width, height);
}

glm::mat4 Renderer::getProjectionMatrix() const
{
	return glm::perspectiveFov(glm::radians(this->fieldOfView), (float)this->width, (float)this->height, this->nearBound, this->farBound);
}

// getter and setters

void Renderer::updateUbo() {
	glm::mat4 projection = this->getProjectionMatrix();

	unsigned int size = 0;
	glBindBuffer(GL_UNIFORM_BUFFER, this->ubo);
	glBufferSubData(GL_UNIFORM_BUFFER, size, sizeof(glm::mat4), glm::value_ptr(this->getProjectionMatrix()));
	size += sizeof(glm::mat4);
	glBufferSubData(GL_UNIFORM_BUFFER, 64, sizeof(glm::vec2), glm::value_ptr(glm::vec2((float) this->width, (float) this->height)));
	size += sizeof(glm::vec2);
	glBufferSubData(GL_UNIFORM_BUFFER, 72, sizeof(float), &this->time);
	size += sizeof(float);
	glBufferSubData(GL_UNIFORM_BUFFER, 76, sizeof(int), &this->gammaCorrection);
	size += sizeof(int);
	glBufferSubData(GL_UNIFORM_BUFFER, 80, sizeof(float), &this->exposure);
	size += sizeof(float);
	glBufferSubData(GL_UNIFORM_BUFFER, 84, sizeof(int), &this->bloom);
	size += sizeof(int);
	glBindBuffer(GL_UNIFORM_BUFFER, 0);
	checkGLError("Scene::updateUbo");
}

void Renderer::setupUbo() {
	int uboSize =
		64 +// perspective matrix
		8 +	// window_size
		4 + // time
		4 + // gamma
		4 +	// exposure
		4 +	// bloom
		8;	// padding
	glGenBuffers(1, &this->ubo);
	glBindBuffer(GL_UNIFORM_BUFFER, this->ubo);
	glBufferData(GL_UNIFORM_BUFFER, uboSize, NULL, GL_STATIC_DRAW);
	glBindBuffer(GL_UNIFORM_BUFFER, 0);
	glBindBufferRange(GL_UNIFORM_BUFFER, 0, this->ubo, 0, uboSize);
	checkGLError("Scene::setupUbo");
}
