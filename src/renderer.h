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

		// debug renders
		void renderVertexNormalLines(Scene* scene);
		void renderTBNLines(Scene* scene);
		void renderVertexFaceLines(Scene* scene);
		void renderDepth(Scene* scene);

		void toggleWireframeMode();

		// getter and setters
		FBOManagerI* getTBM() const;
		void setTBM(FBOManagerI* tbm);

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

    private:
		GLuint ubo;
		FBOManagerI* tbm;
		bool useFBO;
		std::map<std::string, Shader> debugShaders;
        int width, height;
		float time;
		float exposure;
		bool gammaCorrection;
		bool bloom;
		bool shadowsEnabled;

		void setupUbo();
};
