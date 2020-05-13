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

	glViewport(0, 0, width, height);

	checkGLError("Renderer::initialize");

	this->setupUbo();

	this->shaders = {
		//debug
		{"vertexNormalLines", Shader("src/shaders/vertexNormalLines.vert", "src/shaders/vertexNormalLines.frag", "src/shaders/vertexNormalLines.geom").setUniformBlock("Camera", 1) },
		{"faceNormalLines", Shader("src/shaders/faceNormalLines.vert", "src/shaders/faceNormalLines.frag", "src/shaders/faceNormalLines.geom").setUniformBlock("Camera", 1) },
		{"tbnLines", Shader("src/shaders/tbnLines.vert", "src/shaders/tbnLines.frag", "src/shaders/tbnLines.geom").setUniformBlock("Camera", 1)},
		{"linearDepth", Shader("src/shaders/linear_depth.vert", "src/shaders/linear_depth.frag").setUniformBlock("Camera", 1)},
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
		{"gBufferGeometry", Shader("src/shaders/gBuffer.vert", "src/shaders/gBuffer.frag").setUniformBlock("Camera", 1)},
		{"gBufferDLight", Shader("src/shaders/ds_dlight_pass.vert", "src/shaders/ds_dlight_pass.frag").setUniformBlock("Camera", 1).setUniformBlock("Lights", 2).Use().setInt("gPosition", 0).setInt("gNormal", 1).setInt("gAlbedoSpec", 2)},
		{"gBufferPLight", Shader("src/shaders/ds_plight_pass.vert", "src/shaders/ds_plight_pass.frag").setUniformBlock("Camera", 1).Use().setInt("gPosition", 0).setInt("gNormal", 1).setInt("gAlbedoSpec", 2)},
		{"depth", Shader("src/shaders/depth.vert", "src/shaders/depth.frag").setUniformBlock("Camera", 1)},
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
		{"material", Shader("src/shaders/material.vert", "src/shaders/material.frag").setUniformBlock("Camera", 1).setUniformBlock("Lights", 2)},
		{"shadowDepth", Shader("src/shaders/shadowDepth.vert", "src/shaders/shadowDepth.frag")},
		{"shadowCubeDepth", Shader("src/shaders/shadowDepthCube.vert", "src/shaders/shadowDepthCube.frag", "src/shaders/shadowDepthCube.geom")},
		{"shadowDebug2D", Shader("src/shaders/shadowDebug.vert", "src/shaders/shadowDebug.frag").setUniformBlock("Camera", 1).Use().setInt("depthMap", 0)},
		{"shadowCubeDebug", Shader("src/shaders/shadowCubeDebug.vert", "src/shaders/shadowCubeDebug.frag").setUniformBlock("Camera", 1).Use().setInt("depthMap", 1)},
		{"phongLighting", Shader("src/shaders/lighting.vert", "src/shaders/phongLighting.frag").setUniformBlock("Camera", 1).setUniformBlock("Lights", 2)},
		{"directionalShadows", Shader("src/shaders/directionalShadows.vert", "src/shaders/directionalShadows.frag").setUniformBlock("Camera", 1).setUniformBlock("Lights", 2).Use().setInt("shadowMap", 0)},
		{"pointShadows", Shader("src/shaders/pointShadows.vert", "src/shaders/pointShadows.frag").setUniformBlock("Camera", 1).setUniformBlock("Lights", 2).Use().setInt("shadowCubeMap", 1)},
		{"blinnPhongLighting", Shader("src/shaders/lighting.vert", "src/shaders/blinnPhongLighting.frag").setUniformBlock("Scene", 0).setUniformBlock("Camera", 1).setUniformBlock("Lights", 2)},
		{"BPLightingNorm", Shader("src/shaders/BPLightingNorm.vert", "src/shaders/BPLightingNorm.frag").setUniformBlock("Scene", 0).setUniformBlock("Camera", 1).setUniformBlock("Lights", 2)}
	};
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

void Renderer::postRender(Scene* scene)
{

}

void Renderer::render(Scene* scene)
{
	// render shadow depth maps
	if (this->shadowsEnabled) {
		this->renderShadowMaps(scene);
	}

	// render all the models without forward rendering enabled
	auto models = scene->getModels();

	this->gBuffer->BindForWriting();
	for (auto &it : models) {
		if (this->forwardRenderModels.count(it.first) < 1) {
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
	for (auto &it : this->forwardRenderModels) {
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
	scene->getLightManager()->drawLights(this->shaders["light"]);
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

void Renderer::renderDebugTexture(GLuint textureID) {
	if (this->debugVAO == 0)
    {
        float quadVertices[] = {
            // positions        // texture Coords
            -1.0f,  1.0f, 0.0f, 0.0f, 1.0f,
            -1.0f, -1.0f, 0.0f, 0.0f, 0.0f,
             1.0f,  1.0f, 0.0f, 1.0f, 1.0f,
             1.0f, -1.0f, 0.0f, 1.0f, 0.0f,
        };
        // setup plane VAO
        glGenVertexArrays(1, &this->debugVAO);
        glGenBuffers(1, &this->debugVBO);
        glBindVertexArray(this->debugVAO);
        glBindBuffer(GL_ARRAY_BUFFER, this->debugVBO);
        glBufferData(GL_ARRAY_BUFFER, sizeof(quadVertices), &quadVertices, GL_STATIC_DRAW);
        glEnableVertexAttribArray(0);
        glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)0);
        glEnableVertexAttribArray(1);
        glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)(3 * sizeof(float)));

		//cleanup
		glBindVertexArray(0);
		glDeleteBuffers(1, &debugVBO);
    }
	this->shaders["debugTextureQuad"].Use();
	glViewport(this->width * 0.7, this->height * 0.1, this->width * 0.2, this->height * 0.2);
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, textureID);
    glBindVertexArray(this->debugVAO);
    glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);
    glBindVertexArray(0);
	glViewport(0, 0, this->width, this->height);
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
