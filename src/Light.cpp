#include "light.h"

Light::Light(
	glm::vec3 color,
	float ambient,
	float diffuse,
	float specular,
	const std::string prefix
) :
	color(color),
	ambient(ambient),
	diffuse(diffuse),
	specular(specular),
	prefix(prefix)
{
}

PointLight::PointLight(
	glm::vec3 position,
	glm::vec3 color,
	float ambient,
	float diffuse,
	float specular,
	float constant,
	float linear,
	float quadratic,
	const std::string prefix
) :
	Light(color, ambient, diffuse, specular, prefix),
	constant(constant), linear(linear), quadratic(quadratic), position(position)
{ 
	this->color = color;
}

const float PointLight::getRadius() {
	float lightMax = std::fmaxf(std::fmaxf(this->color.r, this->color.g), this->color.b);
	float radius = (-this->linear + std::sqrtf(linear * linear - 4 * quadratic * (constant - (256.0f / 5.0f) * lightMax))) / (2 * quadratic);
	return radius;
}

void PointLight::uploadUniforms(const Shader& shader, const std::string& lightNum) const
{
	shader.setVec3((this->prefix + "light[" + lightNum + "].color"), this->color);
	shader.setFloat((this->prefix + "light[" + lightNum + "].ambient"), this->ambient);
	shader.setFloat((this->prefix + "light[" + lightNum + "].diffuse"), this->diffuse);
	shader.setFloat((this->prefix + "light[" + lightNum + "].specular"), this->specular);
	shader.setFloat((this->prefix + "light[" + lightNum + "].constant"), this->constant);
	shader.setFloat((this->prefix + "light[" + lightNum + "].linear"), this->linear);
	shader.setFloat((this->prefix + "light[" + lightNum + "].quadratic"), this->quadratic);
}

GLuint PointLight::updateUniformBlock(GLuint ubo, GLuint start)
{
	GLuint size = start;
	glBindBuffer(GL_UNIFORM_BUFFER, ubo);
	glBufferSubData(GL_UNIFORM_BUFFER, size, sizeof(float), &this->ambient);
	size += sizeof(float);
	glBufferSubData(GL_UNIFORM_BUFFER, size, sizeof(float), &this->diffuse);
	size += sizeof(float);
	glBufferSubData(GL_UNIFORM_BUFFER, size, sizeof(float), &this->specular);
	size += sizeof(float);
	glBufferSubData(GL_UNIFORM_BUFFER, size, sizeof(float), &this->constant);
	size += sizeof(float);
	glBufferSubData(GL_UNIFORM_BUFFER, size, sizeof(float), &this->linear);
	size += sizeof(float);
	glBufferSubData(GL_UNIFORM_BUFFER, size, sizeof(float), &this->quadratic);
	size += sizeof(float) + 8;
	glBufferSubData(GL_UNIFORM_BUFFER, size, sizeof(glm::vec4), glm::value_ptr(this->color));
	size += sizeof(glm::vec4);
	glBufferSubData(GL_UNIFORM_BUFFER, size, sizeof(glm::vec4), glm::value_ptr(this->position));
	size += sizeof(glm::vec4);
	glBindBuffer(GL_UNIFORM_BUFFER, 0);
	checkGLError("PointLight::updateUniformBlock");
	return size;
}

DirectionLight::DirectionLight(
	glm::vec3 color,
	glm::vec3 direction,
	float ambient,
	float diffuse,
	float specular,
	const std::string prefix
) :
	Light(color, ambient, diffuse, specular, prefix),
	direction(direction)
{
}

void DirectionLight::uploadUniforms(const Shader& shader, const std::string& lightNum) const
{
	shader.setVec3((this->prefix + "light[" + lightNum + "].color"), this->color);
	shader.setFloat((this->prefix + "light[" + lightNum + "].ambient"), this->ambient);
	shader.setFloat((this->prefix + "light[" + lightNum + "].diffuse"), this->diffuse);
	shader.setFloat((this->prefix + "light[" + lightNum + "].specular"), this->specular);
	shader.setVec3((this->prefix + "light[" + lightNum + "].direction").c_str(), this->direction);
}

GLuint DirectionLight::updateUniformBlock(GLuint ubo, GLuint start)
{
	GLuint size = start;
	glBindBuffer(GL_UNIFORM_BUFFER, ubo);
	glBufferSubData(GL_UNIFORM_BUFFER, size, sizeof(float), &this->ambient);
	size += sizeof(float);
	glBufferSubData(GL_UNIFORM_BUFFER, size, sizeof(float), &this->diffuse);
	size += sizeof(float);
	glBufferSubData(GL_UNIFORM_BUFFER, size, sizeof(float), &this->specular);
	size += sizeof(float) + 4;
	glBufferSubData(GL_UNIFORM_BUFFER, size, sizeof(glm::vec4), glm::value_ptr(this->color));
	size += sizeof(glm::vec4);
	glBufferSubData(GL_UNIFORM_BUFFER, size, sizeof(glm::vec4), glm::value_ptr(this->direction));
	size += sizeof(glm::vec4);
	glBindBuffer(GL_UNIFORM_BUFFER, 0);
	checkGLError("DirectionLight::updateUniformBlock");
	return size;
}

SpotLight::SpotLight(
	glm::vec3 position,
	glm::vec3 direction,
	glm::vec3 color,
	float ambient,
	float diffuse,
	float specular,
	float constant,
	float linear,
	float quadratic,
	float cutOff,
	float outerCutOff,
	const std::string prefix
) :
	PointLight(position, color, ambient, diffuse, specular, constant, linear, quadratic, prefix),
	direction(direction), cutOff(cutOff), outerCutOff(outerCutOff)
{
}

void SpotLight::uploadUniforms(const Shader& shader, const std::string& lightNum) const
{
	PointLight::uploadUniforms(shader, lightNum);
	shader.setVec3((this->prefix + "light[" + lightNum + "].direction").c_str(), this->direction);
	shader.setFloat((this->prefix + "light[" + lightNum + "].cutOff").c_str(), this->cutOff);
	shader.setFloat((this->prefix + "light[" + lightNum + "].outerCutOff").c_str(), this->outerCutOff);
}

GLuint SpotLight::updateUniformBlock(GLuint ubo, GLuint start)
{
	GLuint size = start;
	glBindBuffer(GL_UNIFORM_BUFFER, ubo);
	glBufferSubData(GL_UNIFORM_BUFFER, size, sizeof(glm::vec4), glm::value_ptr(this->color));
	size += sizeof(glm::vec4);
	glBufferSubData(GL_UNIFORM_BUFFER, size, sizeof(glm::vec4), glm::value_ptr(this->position));
	size += sizeof(glm::vec4);
	glBufferSubData(GL_UNIFORM_BUFFER, size, sizeof(glm::vec4), glm::value_ptr(this->direction));
	size += sizeof(glm::vec4);
	glBufferSubData(GL_UNIFORM_BUFFER, size, sizeof(float), &this->ambient);
	size += sizeof(float);
	glBufferSubData(GL_UNIFORM_BUFFER, size, sizeof(float), &this->diffuse);
	size += sizeof(float);
	glBufferSubData(GL_UNIFORM_BUFFER, size, sizeof(float), &this->specular);
	size += sizeof(float);
	glBufferSubData(GL_UNIFORM_BUFFER, size, sizeof(float), &this->constant);
	size += sizeof(float);
	glBufferSubData(GL_UNIFORM_BUFFER, size, sizeof(float), &this->linear);
	size += sizeof(float);
	glBufferSubData(GL_UNIFORM_BUFFER, size, sizeof(float), &this->quadratic);
	size += sizeof(float);
	glBufferSubData(GL_UNIFORM_BUFFER, size, sizeof(float), &this->cutOff);
	size += sizeof(float);
	glBufferSubData(GL_UNIFORM_BUFFER, size, sizeof(float), &this->outerCutOff);
	size += sizeof(float);
	glBindBuffer(GL_UNIFORM_BUFFER, 0);
	checkGLError("SpotLight::updateUniformBlock");
	return size;
}
