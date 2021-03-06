#include "camera.h"

Camera::~Camera()
{ }

Camera::Camera(
	glm::vec3 pos, 
	glm::vec3 up, 
	float pitch,
	float yaw
) :
	position(pos), up(up), pitch(pitch), yaw(yaw), front(glm::vec3(0.0f, 0.0f, -1.0f))
{
	setupUbos();
}

glm::mat4 Camera::getViewMatrix() const
{
	return glm::lookAt(this->position, this->position + this->front, this->up);
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

glm::vec3 Camera::getFront() const
{
	return this->front;
}

void Camera::setFront(const glm::vec3& front)
{
	this->front = front;
}

float Camera::getPitch() const {
	return this->pitch;
}

void Camera::setPitch(float pitch) {
	this->pitch = pitch;
}

float Camera::getYaw() const {
	return this->yaw;
}

void Camera::setYaw(float yaw) {
	this->yaw = yaw;
}

void Camera::updateVectors(glm::vec3 worldUp) {
    this->front.x = cos(glm::radians(this->yaw)) * cos(glm::radians(this->pitch));
    this->front.y = sin(glm::radians(this->pitch));
    this->front.z = sin(glm::radians(this->yaw)) * cos(glm::radians(this->pitch));
    // Also re-calculate the Right and Up vector
    // Normalize the vectors, because their length gets closer to 0 the more you look up or down which results in slower movement.
    glm::vec3 right = glm::normalize(glm::cross(this->front, worldUp));  
    this->up = glm::normalize(glm::cross(right, this->front));
}

void Camera::uploadUniforms(Shader& shader)
{
	shader.Use();
	shader.setVec3("camPos", this->position);

	checkGLError("upload camera uniforms -- set camera data");

	glm::mat4 view = this->getViewMatrix();

	shader.setMat4("V", view);

	checkGLError("upload camera uniforms -- matrices");
}

void Camera::updateUniformBlock()
{
	glm::mat4 view = this->getViewMatrix();
	glm::vec3 position = this->getPosition();
	glBindBuffer(GL_UNIFORM_BUFFER, this->uboBlock);
	glBufferSubData(GL_UNIFORM_BUFFER, 0, 64, glm::value_ptr(view));
	glBufferSubData(GL_UNIFORM_BUFFER, 64, 16, glm::value_ptr(position));
	glBindBuffer(GL_UNIFORM_BUFFER, 0);

	checkGLError("Camera::updateUniformBlock");
}

void Camera::setupUbos()
{
	int uboSize = sizeof(glm::mat4) + sizeof(glm::vec4);
	glGenBuffers(1, &this->uboBlock);
	glBindBuffer(GL_UNIFORM_BUFFER, this->uboBlock);
	glBufferData(GL_UNIFORM_BUFFER, uboSize, NULL, GL_STATIC_DRAW);
	glBindBuffer(GL_UNIFORM_BUFFER, 0);
	glBindBufferRange(GL_UNIFORM_BUFFER, 1, this->uboBlock, 0, uboSize);
	checkGLError("Camera::setupUbos");

	this->updateUniformBlock();
}
