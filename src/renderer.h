#pragma once

#include <sstream>
#include <glad/glad.h>
#include "glm/glm.hpp"
#include "glm/gtc/matrix_transform.hpp"
#include "glm/gtc/matrix_inverse.hpp"
#include "glHelper.h"
#include "scene.h"
#include "shader.h"

#define CUBE_TEXTURE_SIZE 256

class Renderer
{
    public:
		Renderer(int width = 1024, int height = 1024);

		// render methods
		void preRender(Scene* scene);
		void render(Scene* scene);
		void renderShadowMaps(Scene* scene);
		void renderLights(Scene* scene);
		void renderSkybox(Scene* scene);

		// defered rendering
		void renderToGBuffer(Scene* scene);

		// debug renders
		void renderVertexNormalLines(Scene* scene);
		void renderTBNLines(Scene* scene);
		void renderVertexFaceLines(Scene* scene);
		void renderDepth(Scene* scene);

		void toggleWireframeMode();

		void updateUbo();

		// getter and setters
		std::unordered_map<std::string, Shader> getShaders() { return this->shaders; }
		const Shader getShader(std::string name) { return this->shaders.at(name); }
		std::unordered_map<std::string, std::string> getForwardRenderModels() { return this->modelShaders; }
		std::string getModelShader(std::string modelName) { return this->modelShaders.count(modelName) ? this->modelShaders.at(modelName) : "Deferred"; }
		FBOManagerI* getTBM() const { return this->tbm; };
		GBuffer* getGBuffer() const { return this->gBuffer; };
		float getNearBound() const { return this->nearBound; }
		float getFarBound() const { return this->farBound; }
		float getFieldOfView() const { return this->fieldOfView; }
		float getTime() const { return this->time; }
		bool getDrawLights() const { return this->drawLights; }
		bool getGammaCorrection() const { return this->gammaCorrection; }
		float getExposure() const { return this->exposure; }
		bool getBloom() const { return this->bloom; }

		void setShaders(std::unordered_map<std::string, Shader> shaders) { this->shaders = shaders; }
		void setShader(std::string name, Shader shader) { this->shaders.at(name) = shader; }
		void setModelShaders(std::unordered_map<std::string, std::string> map) { this->modelShaders = map; }
		void setModelShader(std::string model, std::string shader) { this->modelShaders.insert_or_assign(model, shader); }
		void removeModelShader(std::string modelName) { this->modelShaders.erase(modelName); }
		void setTBM(FBOManagerI* tbm) { this->tbm = tbm; };
		void setGBuffer(GBuffer* gBuffer) { this->gBuffer = gBuffer; };
		void setNearBound(float nearBound) { this->nearBound = nearBound; }
		void setFarBound(float farBound) { this->farBound = farBound; }
		void setFieldOfView(float fieldOfView) { this->fieldOfView = fieldOfView; }
		void setTime(float time) { this->time = time; }
		void setDrawLights(bool drawLights) { this->drawLights = drawLights; }
		void setGammaCorrection(bool gamma) { this->gammaCorrection = gamma; }
		void setExposure(float exposure) { this->exposure = exposure; }
		void setBloom(bool bloom) { this->bloom = bloom; }
		void setDimensions(int width, int height);

		glm::mat4 getProjectionMatrix() const;

    private:
		GLuint ubo;
		GLuint debugVAO = 0, debugVBO;
		FBOManagerI* tbm;
		GBuffer* gBuffer;
		std::unordered_map<std::string, Shader> shaders;

		///<summary>first: The name of the model in the scene, second: the name of the shader
		///<para>controls whether or not the model will be ommited from the gBuffer render pass, and then rendered afterwords with the specified shader</para>
		///</summary>
		std::unordered_map<std::string, std::string> modelShaders;
		float nearBound, farBound, fieldOfView;
        int width, height;
		float time;
		float exposure;

		bool drawLights;
		bool renderShadows;
		bool useFBO;
		bool gammaCorrection;
		bool bloom;

		void setupUbo();
};
