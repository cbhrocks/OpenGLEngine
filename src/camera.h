#pragma once

#include <glad/glad.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include "glHelper.h"
#include "shader.h"
#include "FBOManager.h"

class Camera
{
public:
	glm::vec3 position;
	glm::vec3 look;
	glm::vec3 up;
	GLuint uboBlock;
	float farBound;
	float nearBound;
	float aspectRatio;
	float fov;
	FBOManagerI* tbm;

	Camera();
	Camera(glm::vec3 pos, glm::vec3 look, glm::vec3 up, float farBound, float nearBound, float aspectRatio, float fov);

	glm::mat4 getProjectionMatrix() const;

	glm::mat4 getViewMatrix() const;

	glm::vec3 getPosition() const;
	void setPosition(const glm::vec3& pos);

	glm::vec3 getUp() const;
	void setUp(const glm::vec3& up);

	glm::vec3 getLook() const;
	void setLook(const glm::vec3& look);

	FBOManagerI* getTBM() const;
	void setTBM(FBOManagerI* tbm);

	void uploadUniforms(Shader& shader);
	void updateUniformBlock();

	~Camera();

private:
	void setupUbos();
};