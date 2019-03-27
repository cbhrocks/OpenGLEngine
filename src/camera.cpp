#include "camera.h"

Camera::Camera() 
{ }

Camera::~Camera()
{ }

Camera::Camera(glm::vec3 pos, glm::vec3 look, glm::vec3 up, float farBound, float nearBound, float aspectRatio, float fov) :
	position(pos), look(look), up(up), farBound(farBound), nearBound(nearBound), aspectRatio(aspectRatio), fov(fov)
{
	setupUbos();
}

glm::mat4 Camera::getProjectionMatrix() const
{
	return glm::perspective(this->fov, this->aspectRatio, this->nearBound, this->farBound); // using aspect ratio
	//return glm::perspectiveFov(1.0f, (float) this->width, (float) this->height, this->near, this->far); // using width and height
}

glm::mat4 Camera::getViewMatrix() const
{
	return glm::lookAt(this->position, this->position + this->look, this->up);
}


FBOManagerI* Camera::getTBM() const
{
	return this->tbm;
}

void Camera::setTBM(FBOManagerI* tbm)
{
	this->tbm = tbm;
}

glm::vec3 Camera::getPosition() const
{
	return this->position;
}

void Camera::setPosition(const glm::vec3& pos)
{
	this->position = pos;
}

glm::vec3 Camera::getUp() const
{
	return this->up;
}

void Camera::setUp(const glm::vec3& up)
{
	this->up = up;
}

glm::vec3 Camera::getLook() const
{
	return this->look;
}

void Camera::setLook(const glm::vec3& look)
{
	this->look = look;
}

void Camera::uploadUniforms(Shader& shader)
{
	shader.Use();
	shader.setFloat("near", this->nearBound);
	shader.setFloat("far", this->farBound);
	shader.setFloat("fov", this->fov);
	shader.setVec3("camPos", this->position);

	checkGLError("upload camera uniforms -- set camera data");

	glm::mat4 projection = this->getProjectionMatrix();
	glm::mat4 view = this->getViewMatrix();

	shader.setMat4("P", projection);
	shader.setMat4("V", view);

	checkGLError("upload camera uniforms -- matrices");
}

void Camera::updateUniformBlock()
{
	glm::mat4 view = this->getViewMatrix();
	glm::vec3 position = this->getPosition();
	glBindBuffer(GL_UNIFORM_BUFFER, this->uboBlock);
	glBufferSubData(GL_UNIFORM_BUFFER, sizeof(glm::mat4), sizeof(glm::mat4), glm::value_ptr(view));
	glBufferSubData(GL_UNIFORM_BUFFER, 2 * sizeof(glm::mat4), sizeof(glm::vec4), glm::value_ptr(position));
	glBindBuffer(GL_UNIFORM_BUFFER, 0);

	checkGLError("Camera::updateUniformBlock");
}

void Camera::setupUbos()
{
	int uboSize = 2 * sizeof(glm::mat4) + sizeof(glm::vec4);
	glGenBuffers(1, &this->uboBlock);
	glBindBuffer(GL_UNIFORM_BUFFER, this->uboBlock);
	glBufferData(GL_UNIFORM_BUFFER, uboSize, NULL, GL_STATIC_DRAW);
	glBindBuffer(GL_UNIFORM_BUFFER, 0);
	glBindBufferRange(GL_UNIFORM_BUFFER, 1, this->uboBlock, 0, uboSize);
	checkGLError("Camera::setupUbos");

	//set initial value
	glm::mat4 projection = this->getProjectionMatrix();
	glBindBuffer(GL_UNIFORM_BUFFER, this->uboBlock);
	glBufferSubData(GL_UNIFORM_BUFFER, 0, sizeof(glm::mat4), glm::value_ptr(projection));
	glBindBuffer(GL_UNIFORM_BUFFER, 0);
	checkGLError("Camera::setupUbos");

	this->updateUniformBlock();
}
