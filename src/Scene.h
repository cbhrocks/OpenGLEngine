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
	size_t width, height;
	double currentTime;
	std::map<std::string, Shader> shaders;
	bool gammaCorrection;
	float exposure;
	Tex2DRenderer tex2DR;

	Scene();
	void loadObjects();
	void clearObjects();

	void onFrame();

	void uploadSkyboxUniforms(Shader shader);

	void draw();
	void drawShadows();
	void drawSkybox();
	void drawModels(Shader shader);
	void drawModels();
	void drawHighlight();

	void timeStep(double t);

	Camera* getActiveCamera() const;
	void setActiveCamera(const int &activeCamera);
	void addCamera(Camera* camera);

	glm::vec3 getUp() const;
	void setUp(glm::vec3 up);

	size_t getWidth() const;
	void setWidth(size_t width);
	size_t getHeight() const;
	void setHeight(size_t height);

	void setGammaCorrection(bool gamma);
	bool getGammaCorrection() const;

	void setExposure(float exposure);
	float getExposure() const;

	void scaleModels(const glm::vec3& scale);

	void initializeShaders();

private:
	GLuint ubo;
	Skybox* skybox;
	LightManager* lightManager;
	std::vector<Model*> models;
	std::vector<DrawObject*> drawObjects;
	std::vector<Camera*> cameras;
	int activeCamera;
	bool running;
	Shader lightShader;

	void setup();

	void setupUbo();
	void updateUbo();
};