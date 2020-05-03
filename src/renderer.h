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
		void postRender(Scene* scene);
		void render(Scene* scene);
		void renderShadows(Scene* scene);
		void renderModels(Scene* scene);
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

		// getter and setters
		FBOManagerI* getTBM() const;
		void setTBM(FBOManagerI* tbm);

		GBuffer* getGBuffer() const;
		void setGBuffer(GBuffer* gBuffer);

		void setTime(float time);
		float getTime() const;

		void setGammaCorrection(bool gamma);
		bool getGammaCorrection() const;

		void setExposure(float exposure);
		float getExposure() const;

		void setBloom(bool bloom);
		bool getBloom() const;

		void setRes(int width, int height);
		void updateUbo();

		void setModelShader(std::string model, std::string shader) {
			this->forwardRenderModels.insert_or_assign(model, shader);
		}

    private:
		GLuint ubo;
		FBOManagerI* tbm;
		GBuffer* gBuffer;
		std::unordered_map<std::string, const Shader> shaders;

		///<summary>first: The name of the model in the scene, second: the name of the shader
		///<para>controls whether or not the model will be ommited from the gBuffer render pass, and then rendered afterwords with the specified shader</para>
		///</summary>
		std::unordered_map<std::string, std::string> forwardRenderModels;
        int width, height;
		float time;
		float exposure;
		bool useFBO;
		bool gammaCorrection;
		bool bloom;
		bool shadowsEnabled;

		void setupUbo();
};
