#ifndef SCENE_H
#define SCENE_H

#include <glad/glad.h>
#include <iostream>
#include "model.h"
#include "light.h"
#include "shader.h"
#include "plane.h"
#include "skybox.h"
#include "cube.h"

class Camera {
public:
	glm::vec3 position;
	glm::vec3 look;
	glm::vec3 up;
	GLuint uboBlock;
	float farBound;
	float nearBound;
	float aspectRatio;
	float fov;

	Camera() {
	}

	Camera(glm::vec3 pos, glm::vec3 look, glm::vec3 up, float farBound, float nearBound, float aspectRatio, float fov) :
		position(pos), look(look), up(up), farBound(farBound), nearBound(nearBound), aspectRatio(aspectRatio), fov(fov)
	{
		setupUbo();
	}

	glm::mat4 getProjectionMatrix()
	{
		return glm::perspective(this->fov, this->aspectRatio, this->nearBound, this->farBound); // using aspect ratio
		//return glm::perspectiveFov(1.0f, (float) this->width, (float) this->height, this->near, this->far); // using width and height
	}

	glm::mat4 getViewMatrix() const
	{
		return glm::lookAt(this->position, this->position + this->look, this->up);
	}

	void uploadUniforms(Shader shaer) 
	{

	}

	void bindUniformBlock(Shader shader)
	{
		glUniformBlockBinding(shader.getId(), this->uboBlock, 0);
	}

	void updateUniformBlock()
	{
		glm::mat4 view = this->getViewMatrix();
		glBindBuffer(GL_UNIFORM_BUFFER, uboBlock);
		glBufferSubData(GL_UNIFORM_BUFFER, sizeof(glm::mat4), sizeof(glm::mat4), glm::value_ptr(view));
		glBindBuffer(GL_UNIFORM_BUFFER, 0);
	}

	void setPos(glm::vec3 pos)
	{
		this->position = pos;
	}

	void setPos(glm::vec3 pos, glm::vec3 look, glm::vec3 up)
	{
		this->position = pos;
		this->look = look;
		this->up = up;
	}

private:
	void setupUbo() 
	{
		int uboSize = 2 * sizeof(glm::mat4);
		glGenBuffers(1, &this->uboBlock);
		glBindBuffer(GL_UNIFORM_BUFFER, this->uboBlock);
		glBufferData(GL_UNIFORM_BUFFER, uboSize, NULL, GL_STATIC_DRAW);
		glBindBuffer(GL_UNIFORM_BUFFER, 0);
		glBindBufferRange(GL_UNIFORM_BUFFER, 0, this->uboBlock, 0, uboSize);

		//set initial value
		glm::mat4 projection = this->getProjectionMatrix();
		glBindBuffer(GL_UNIFORM_BUFFER, uboBlock);
		glBufferSubData(GL_UNIFORM_BUFFER, 0, sizeof(glm::mat4), glm::value_ptr(projection));
		glBindBuffer(GL_UNIFORM_BUFFER, 0);
	}
};

class Scene {
public:
	glm::vec3 up;
	Camera camera;
	double currentTime;
	std::map<std::string, Shader> shaders;

	Scene() {}

	Scene(float aspectRatio)
	{
		running = true;

		this->up = glm::vec3(0.0, 1.0, 0.0);
		this->camera = Camera(glm::vec3(0, 0, 3), glm::vec3(0, 0, -1), glm::vec3(0, 1, 0), 100.0f, 0.1f, aspectRatio, 45.0f);

		std::vector<std::string> faces{
			"objects/test/textures/skybox/right.jpg",
			"objects/test/textures/skybox/left.jpg",
			"objects/test/textures/skybox/top.jpg",
			"objects/test/textures/skybox/bottom.jpg",
			"objects/test/textures/skybox/front.jpg",
			"objects/test/textures/skybox/back.jpg"
		};
		skybox = Skybox(faces);

			//model = Model(std::string("objects/test/sibenik.obj"));
		model = Model(std::string("objects/test/nanosuit/nanosuit.obj"));
		Plane* floor = new Plane(std::string("objects/test/textures/metal.png"));
		floor->setRotation(glm::vec3(-90.0, 0.0, 0.0));
		floor->setScale(glm::vec3(25.0));
		this->planes.push_back(floor);
		this->cube = Cube(std::string("objects/test/textures/container.jpg"));
		this->cube.setScale(glm::vec3(3));
		this->cube.setPosition(glm::vec3(-4.0, 1.5, 3.5));
		this->cube.setSkybox(this->skybox.getTexture());
		Plane* grass = new Plane(std::string("objects/test/textures/grass.png"));
		grass->setPosition(glm::vec3(0.0, 0.5, 1.0));
		this->planes.push_back(grass);
		Plane* window1 = new Plane(std::string("objects/test/textures/blending_transparent_window.png"));
		window1->setPosition(glm::vec3(0.5, 0.5, 2.0));
		this->planes.push_back(window1);
		Plane* window2 = new Plane(std::string("objects/test/textures/blending_transparent_window.png"));
		window2->setPosition(glm::vec3(0.5, 0.5, 3.0));
		this->planes.push_back(window2);
		//model = Model(std::string("objects/inanimate/tiles/hex-sides.obj"));

		GLchar const *lightVPath = "src/shaders/light.vert";
		GLchar const *lightFPath = "src/shaders/light.frag";
		lightShader = Shader(lightVPath, lightFPath);

		BasicLight* basicLight = new BasicLight(
			glm::vec3(0.2f, 0.2f, 0.2f),
			glm::vec3(0.5f, 0.5f, 0.5f),
			glm::vec3(1.0f, 1.0f, 1.0f),
			glm::vec3(0.0f, 5.0f, 5.0f)
		);
		this->basicLights.push_back(basicLight);
		DirectionLight* directionLight = new DirectionLight(
			glm::vec3(0.2f, 0.2f, 0.2f),
			glm::vec3(0.5f, 0.5f, 0.5f),
			glm::vec3(1.0f, 1.0f, 1.0f),
			glm::vec3(0.0f, -1.0f, 0.0f)
		);
		this->directionLights.push_back(directionLight);
		PointLight* pointLight = new PointLight(
			glm::vec3(0.2f, 0.2f, 0.2f),
			glm::vec3(0.5f, 0.5f, 0.5f),
			glm::vec3(1.0f, 1.0f, 1.0f),
			glm::vec3(0.0f, 8.0f, 5.0f),
			1.0f,
			0.09f,
			0.032f
		);
		this->pointLights.push_back(pointLight);
		SpotLight* spotLight = new SpotLight(
			glm::vec3(0.2f, 0.2f, 0.2f),
			glm::vec3(0.5f, 0.5f, 0.5f),
			glm::vec3(1.0f, 1.0f, 1.0f),
			//glm::vec3(0.0f, 10.0f, 5.0f),
			this->camera.position,
			glm::vec3(0.0f, 0.0f, -1.0f),
			1.0f,
			0.14f,
			0.07f,
			glm::cos(glm::radians(12.5f)),
			glm::cos(glm::radians(17.5f))
		);
		this->spotLights.push_back(spotLight);
	}

	//void uploadUniforms(Shader shader) {
		//this->uploadCameraUniforms(shader);
		//this->uploadLightUniforms(shader);
		//this->uploadModelUniforms(shader);
	//}

	void onFrame()
	{
		this->camera.updateUniformBlock();
		this->spotLights[0]->setPos(this->camera.position);
		this->spotLights[0]->setDirection(this->camera.look);
		this->pointLights[0]->setPos(glm::vec4(pointLights[0]->GetPos(), 1.0f) * glm::rotate(glm::mat4(1.0f), glm::radians(0.25f), glm::vec3(0.0f, 1.0f, 0.0f)));
	}

	void uploadCameraUniforms(Shader shader) {
		shader.setFloat("near", this->camera.nearBound);
		shader.setFloat("far", this->camera.farBound);
		shader.setFloat("fov", this->camera.fov);
		shader.setVec3("camPos", this->camera.position);

		checkGLError("upload camera uniforms -- set camera data");

		glm::mat4 projection = this->camera.getProjectionMatrix();
		glm::mat4 view = this->camera.getViewMatrix();

		shader.setMat4("P", projection);
		shader.setMat4("V", view);

		checkGLError("upload camera uniforms -- matrices");

		this->model.uploadUniforms(shader);

		checkGLError("upload camera uniforms -- light colors");
	}

	void uploadSkyboxUniforms(Shader shader) {
		shader.setInt("texture1", 0);

		glm::mat4 projection = this->camera.getProjectionMatrix();
		glm::mat4 view = glm::mat4(glm::mat3(this->camera.getViewMatrix()));

		shader.setMat4("P", projection);
		shader.setMat4("V", view);

		checkGLError("upload skybox uniforms");
	}

	void drawSkybox(Shader shader) {
		uploadSkyboxUniforms(shader);
		skybox.Draw(shader);
	}

	void uploadLightUniforms(Shader shader) {
		for (int i = 0; i < this->pointLights.size(); i++) {
			string lightNum = std::to_string(i);
			this->pointLights[i]->uploadUniforms(shader, lightNum);
		}
		for (int i = 0; i < this->basicLights.size(); i++) {
			string lightNum = std::to_string(i);
			this->basicLights[i]->uploadUniforms(shader, lightNum);
		}
		for (int i = 0; i < this->directionLights.size(); i++) {
			string lightNum = std::to_string(i);
			this->directionLights[i]->uploadUniforms(shader, lightNum);
		}
		for (int i = 0; i < this->spotLights.size(); i++) {
			string lightNum = std::to_string(i);
			this->spotLights[i]->uploadUniforms(shader, lightNum);
		}
	}

	void drawLights()
	{
		lightShader.Use();
		uploadCameraUniforms(lightShader);
		uploadLightUniforms(lightShader);
		for (int i = 0; i < this->pointLights.size(); i++) {
			this->pointLights[i]->Draw(lightShader);
		}
		for (int i = 0; i < this->basicLights.size(); i++) {
			this->basicLights[i]->Draw(lightShader);
		}
		//for(int i = 0; i < this->directionLights.size(); i++){
		//    this->directionLights[i]->Draw(lightShader);
		//}
		//for(int i = 0; i < this->spotLights.size(); i++){
		//    this->spotLights[i]->Draw(lightShader);
		//}
	}

	void drawModels(const Shader& shader)
	{
		this->uploadCameraUniforms(shader);
		this->uploadLightUniforms(shader);
		this->model.uploadUniforms(shader);
		this->model.Draw(shader);
		this->drawTransparentModels(shader);
	}

	void drawReflections(const Shader& shader)
	{
		this->uploadCameraUniforms(shader);
		this->cube.uploadUniforms(shader);
		this->cube.Draw();
	}

	void drawTransparentModels(const Shader& shader)
	{
		std::map<float, Plane*> sorted;
		for (int i = 0; i < this->planes.size(); i++)
		{
			float distance = glm::length(this->camera.position - planes[i]->position);
			sorted[distance] = planes[i];
		}

		for (std::map<float, Plane*>::reverse_iterator it = sorted.rbegin(); it != sorted.rend(); ++it)
		{
			it->second->uploadUniforms(shader);
			it->second->Draw(shader);
		}
	}

	void timeStep(double t)
	{
		this->currentTime = t;
	}

	Model const & getModel() const
	{
		return model;
	}

	glm::mat4 getViewMatrix() const
	{
		return this->camera.getViewMatrix();
	}

	glm::vec3 getCameraUp() const
	{
		return this->camera.up;
	}

	glm::vec3 getCameraPos() const
	{
		return this->camera.position;
	}

	glm::vec3 getCameraLook() const
	{
		return this->camera.look;
	}

	void scaleModels(glm::vec3 scale)
	{
		this->model.setScale(scale);
	}

	void setCameraPos(glm::vec3 pos)
	{
		this->camera.setPos(pos);
	}

	void setCameraPos(glm::vec3 pos, glm::vec3 look, glm::vec3 up)
	{
		this->camera.setPos(pos, look, up);
	}

	void setCamView(glm::vec3 look, glm::vec3 up)
	{
		this->camera.look = look;
		this->camera.up = up;
	}

	void initializeShaders()
	{
		this->shaders["basic"] = Shader("src/shaders/basic.vert", "src/shaders/basic.frag");
	}

private:
	Model model;
	Cube cube;
	Skybox skybox;
	std::vector<Plane*> planes;
	bool running;
	Shader lightShader;

	glm::vec3 startPoint;
	glm::vec3 endPoint;
	std::vector<BasicLight*> basicLights;
	std::vector<PointLight*> pointLights;
	std::vector<DirectionLight*> directionLights;
	std::vector<SpotLight*> spotLights;
};

#endif