#include "DrawObj.h"

DrawObject::DrawObject() :
	Shader(0)
{
}

DrawObject::DrawObject(
	Shader& shader,
	std::vector<glm::vec3> positions,
	std::vector<glm::vec3> normals,
	std::vector<glm::vec2> texCoords,
	std::vector<glm::vec3> tangents,
	std::vector<glm::vec3> bitangents,
	std::vector<GLuint> indices,
	std::vector<Texture> textures,
	Texture* skybox,
	bool transparent,
	bool highlight,
	GLfloat reflect
) :
	shader(shader),
	Positions(positions),
	Normals(normals),
	TexCoords(texCoords),
	Tangents(tangents),
	Bitangents(bitangents),
	Textures(textures),
	Indices(indices),
	Skybox(skybox),
	transparent(transparent),
	highlight(highlight),
	reflect(reflect),
	rotation(0.0),
	position(0.0),
	scale(1.0)
{
	//cout << "skybox stuff " << skybox->id << endl;
	this->setup();
}

// render the mesh
void DrawObject::UploadUniforms()
{
	this->UploadUniforms(this->shader);
}

void DrawObject::UploadUniforms(Shader& shader)
{
	shader.Use();

	shader.setFloat("reflection", this->reflect);

	glm::mat4 model = glm::mat4(1.0f);
	model = glm::translate(model, this->position);
	//rotate x
	model = glm::rotate(model, glm::radians(this->rotation.x), glm::vec3(1.0, 0.0, 0.0));
	//rotate y
	model = glm::rotate(model, glm::radians(this->rotation.y), glm::vec3(0.0, 1.0, 0.0));
	//rotate z
	model = glm::rotate(model, glm::radians(this->rotation.z), glm::vec3(0.0, 0.0, 1.0));
	model = glm::scale(model, this->scale);
	glm::mat3 normal = glm::inverseTranspose(glm::mat3(model));

	shader.setMat4("Model", model);
	shader.setMat3("Normal", normal);
	checkGLError("DrawObject::UploadUniforms");
}

void DrawObject::Draw()
{
	this->Draw(this->shader);
};

void DrawObject::Draw(Shader& shader)
{
	shader.Use();

	// bind appropriate textures
	GLuint diffuseNr = 1;
	GLuint specularNr = 1;
	GLuint normalNr = 1;
	GLuint heightNr = 1;
	GLuint reflectNr = 1;
	GLuint shadowDirectionNr = 1;
	GLuint shadowOmniNr = 1;
	GLuint i;
	for (i = 0; i < Textures.size(); i++)
	{
		glActiveTexture(GL_TEXTURE0 + i); // active proper texture unit before binding
		// retrieve texture number (the N in diffuse_textureN)
		std::string number;
		std::string name = Textures[i].type;
		if (name == "texture_diffuse")
			number = std::to_string(diffuseNr++);
		else if (name == "texture_specular")
			number = std::to_string(specularNr++); // transfer unsigned int to stream
		else if (name == "texture_normal")
			number = std::to_string(normalNr++); // transfer unsigned int to stream
		else if (name == "texture_height")
			number = std::to_string(heightNr++); // transfer unsigned int to stream
		else if (name == "texture_reflect")
			number = std::to_string(reflectNr++); // transfer unsigned int to stream
		else if (name == "texture_shadow_direction")
			number = std::to_string(shadowDirectionNr++);
		else if (name == "texture_shadow_cube")
			number = std::to_string(shadowOmniNr++);

		// now set the sampler to the correct texture unit
		shader.setInt(("material." + name + number).c_str(), i);
		// and finally bind the texture
		glBindTexture(name == "texture_shadow_cube" ? GL_TEXTURE_CUBE_MAP : GL_TEXTURE_2D, Textures[i].id);
		checkGLError("DrawObject::Draw -- bind textures");
	}

	if (Skybox != nullptr) {
		glActiveTexture(GL_TEXTURE0 + ++i);
		shader.setInt("skybox", i);
		glBindTexture(GL_TEXTURE_CUBE_MAP, this->Skybox->id);
	}

	// draw mesh
	glBindVertexArray(VAO);
	glDrawArrays(GL_TRIANGLES, 0, Positions.size());

	//unset opengl variables
	// always good practice to set everything back to defaults once configured.
	glBindVertexArray(0);
	glActiveTexture(GL_TEXTURE0);

	checkGLError("DrawObj::Draw -- draw vao");
}

Shader& DrawObject::getShader() const { return this->shader; }
void DrawObject::setShader(Shader& shader) { this->shader = shader; }

bool DrawObject::isTransparent() const { return this->transparent; }
void DrawObject::setTransparent(bool transparent) { this->transparent = transparent; }

bool DrawObject::canHighlight() const { return this->highlight; }
void DrawObject::setHighlight(bool highlight) { this->highlight = highlight; }

GLfloat DrawObject::getReflect() const { return this->reflect; }
void DrawObject::setReflective(GLfloat& reflect) { this->reflect = reflect; }

//positioning functions
void DrawObject::setPosition(glm::vec3 position) { this->position = position; }
glm::vec3 DrawObject::getPosition() const { return this->position; }

void DrawObject::setScale(glm::vec3 scale) { this->scale = scale; }
glm::vec3 DrawObject::getScale() const { return this->scale; }

void DrawObject::setRotation(glm::vec3 rotation) { this->rotation = rotation; }
glm::vec3 DrawObject::getRotation() const { return this->rotation; }
void DrawObject::addRotation(glm::vec3 rotation) { this->rotation += rotation; }

void DrawObject::calculateTanAndBitan()
{
	std::vector<glm::vec3> tangents;
	std::vector<glm::vec3> bitangents;
	for (int i = 0; i < this->Positions.size(); i+=3) {
		glm::vec3 edge1 = this->Positions.at(i + 1) - this->Positions.at(i);
		glm::vec3 edge2 = this->Positions.at(i + 2) - this->Positions.at(i);
		glm::vec2 deltaUV1 = this->TexCoords.at(i + 1) - this->TexCoords.at(i);
		glm::vec2 deltaUV2 = this->TexCoords.at(i + 2) - this->TexCoords.at(i);

		float f = 1.0f / (deltaUV1.x * deltaUV2.y - deltaUV2.x * deltaUV1.y);

		glm::vec3 tangent;
		tangent.x = f * (deltaUV2.y * edge1.x - deltaUV1.y * edge2.x);
		tangent.y = f * (deltaUV2.y * edge1.y - deltaUV1.y * edge2.y);
		tangent.z = f * (deltaUV2.y * edge1.z - deltaUV1.y * edge2.z);
		tangent = glm::normalize(tangent);
		tangents.push_back(tangent);
		tangents.push_back(tangent);
		tangents.push_back(tangent);

		glm::vec3 bitangent;
		bitangent.x = f * (-deltaUV2.x * edge1.x + deltaUV1.x * edge2.x);
		bitangent.y = f * (-deltaUV2.x * edge1.y + deltaUV1.x * edge2.y);
		bitangent.z = f * (-deltaUV2.x * edge1.z + deltaUV1.x * edge2.z);
		bitangent = glm::normalize(bitangent);
		bitangents.push_back(bitangent);
		bitangents.push_back(bitangent);
		bitangents.push_back(bitangent);
	}
	this->Tangents = tangents;
	this->Bitangents = bitangents;
}

/*  Functions    */
// initializes all the buffer objects/arrays
void DrawObject::setup()
{
	if (this->Positions.size() > 0 && this->TexCoords.size() > 0) {
		this->calculateTanAndBitan();
	}

	glGenVertexArrays(1, &VAO);
	glGenBuffers(1, &VBO);

	glBindVertexArray(VAO);
	glBindBuffer(GL_ARRAY_BUFFER, VBO);
	checkGLError("DrawObjectect create buffers");

	GLintptr positionsSize = Positions.size() * sizeof(glm::vec3),
		normalsSize = Normals.size() * sizeof(glm::vec3),
		texCoordsSize = TexCoords.size() * sizeof(glm::vec2),
		tangentsSize = Tangents.size() * sizeof(glm::vec3),
		bitangentsSize = Bitangents.size() * sizeof(glm::vec3);
	GLintptr size = 0;
	checkGLError("DrawObjectect setup sizes");

	glBufferData(GL_ARRAY_BUFFER, (positionsSize + normalsSize + texCoordsSize + tangentsSize + bitangentsSize), &Positions.front(), GL_STATIC_DRAW);

	if (Positions.size() > 0) {
		glBufferSubData(GL_ARRAY_BUFFER, 0, Positions.size() * sizeof(glm::vec3), &Positions.front());
		glEnableVertexAttribArray(0);
		glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(glm::vec3), (void*)size);
		size += positionsSize;
		checkGLError("DrawObjectect setup position attribs");
	}

	if (Normals.size() > 0) {
		glBufferSubData(GL_ARRAY_BUFFER, size, Normals.size() * sizeof(glm::vec3), &Normals.front());
		glEnableVertexAttribArray(1);
		glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(glm::vec3), (void*)size);
		size += normalsSize;
		checkGLError("DrawObjectect setup normals attribs");
	}

	if (TexCoords.size() > 0) {
		glBufferSubData(GL_ARRAY_BUFFER, size, TexCoords.size() * sizeof(glm::vec2), &TexCoords.front());
		glEnableVertexAttribArray(2);
		glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, sizeof(glm::vec2), (void*)size);
		size += texCoordsSize;
		checkGLError("DrawObjectect setup texcoords attribs");
	}

	if (Tangents.size() > 0) {
		glBufferSubData(GL_ARRAY_BUFFER, size, Tangents.size() * sizeof(glm::vec3), &Tangents.front());
		glEnableVertexAttribArray(3);
		glVertexAttribPointer(3, 3, GL_FLOAT, GL_FALSE, sizeof(glm::vec3), (void*)size);
		size += tangentsSize;
		checkGLError("DrawObjectect setup tangents attribs");
	}

	if (Bitangents.size() > 0) {
		glBufferSubData(GL_ARRAY_BUFFER, size, Bitangents.size() * sizeof(glm::vec3), &Bitangents.front());
		glEnableVertexAttribArray(4);
		glVertexAttribPointer(4, 3, GL_FLOAT, GL_FALSE, sizeof(glm::vec3), (void*)size);
		checkGLError("DrawObjectect setup bitangents attribs");
	}

	glBindVertexArray(0);
}

	DrawObject::builder::builder() {}

	DrawObject::builder& DrawObject::builder::setPositions(std::vector<glm::vec3> positions) { this->Positions = positions; return *this; }
	DrawObject::builder& DrawObject::builder::setNormals(std::vector<glm::vec3> normals) { this->Normals = normals; return *this; }
	DrawObject::builder& DrawObject::builder::setTexCoords(std::vector<glm::vec2> texCoords) { this->TexCoords = texCoords; return *this; }
	DrawObject::builder& DrawObject::builder::setTangents(std::vector<glm::vec3> tangents) { this->Tangents = tangents; return *this; }
	DrawObject::builder& DrawObject::builder::setBitangents(std::vector<glm::vec3> bitangents) { this->Bitangents = bitangents; return *this; }
	DrawObject::builder& DrawObject::builder::setIndices(std::vector<GLuint> indices) { this->Indices = indices; return *this; }
	DrawObject::builder& DrawObject::builder::setTextures(std::vector<Texture> textures) { this->Textures = textures;  return *this; }
	DrawObject::builder& DrawObject::builder::setSkybox(Texture* skybox) { this->Skybox = skybox; return *this; }
	DrawObject::builder& DrawObject::builder::setReflect(GLfloat reflect) { this->reflect = reflect; return *this; }
	DrawObject::builder& DrawObject::builder::setTransparent(bool transparent) { this->transparent = transparent; return *this; }
	DrawObject::builder& DrawObject::builder::setHighlight(bool highlight) { this->highlight = highlight; return *this; }

	DrawObject* DrawObject::builder::build(Shader& shader) const
	{
		return new DrawObject(
			shader, 
			this->Positions, 
			this->Normals, 
			this->TexCoords, 
			this->Tangents, 
			this->Bitangents, 
			this->Indices, 
			this->Textures, 
			this->Skybox, 
			this->transparent, 
			this->highlight,
			this->reflect
		);
	}
