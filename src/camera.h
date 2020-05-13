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
	glm::vec3 front;
	glm::vec3 up;
	GLuint uboBlock;
	float farBound;
	float nearBound;
	float yaw;
	float pitch;
	float width;
	float height;
	float fov;

	Camera(
		glm::vec3 pos,
		glm::vec3 up,
		float pitch,
		float yaw
	);

	glm::mat4 getProjectionMatrix() const;

	glm::mat4 getViewMatrix() const;

	const glm::vec3 getPosition() const { return this->position; };
	const glm::vec3& getPositionRef() const { return this->position; };
	void setPosition(const glm::vec3& pos);

	glm::vec3 getUp() const;
	void setUp(const glm::vec3& up);

	glm::vec3 getFront() const;
	void setFront(const glm::vec3& look);

	float getPitch() const;
	void setPitch(float pitch);

	float getYaw() const;
	void setYaw(float yaw);

	void uploadUniforms(Shader& shader);
	void updateUniformBlock();

	void updateVectors(glm::vec3 worldUp);

	~Camera();

private:
	void setupUbos();
};
