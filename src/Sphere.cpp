#include "Sphere.h"

Sphere::Sphere(std::string name, float radius, int sectorCount, int stackCount, bool smooth) : IDrawObj(name) {
	this->getMaterial()->AmbientColor = glm::vec4(0.0f, 1.0f, 0.0f, 1.0f);
	this->buildSphere(radius, sectorCount, stackCount, smooth);
}

void Sphere::buildSphere(float radius, int sectorCount, int stackCount, bool smooth) {
	this->radius = radius;
	this->sectorCount = sectorCount;
	if (sectorCount < MIN_SECTOR_COUNT)
		this->sectorCount = MIN_SECTOR_COUNT;
	this->stackCount = stackCount;
	if (stackCount < MIN_STACK_COUNT)
		this->stackCount = MIN_STACK_COUNT;

	this->smooth = smooth;
	if (smooth) {
		this->buildVerticesSmooth();
	}
	else {
		this->buildVerticesFlat();
	}
}

////////////////////////////////////////////////////////////////////////////////////////////
// setters
////////////////////////////////////////////////////////////////////////////////////////////

void Sphere::setRadius(float radius) {
	float scale = sqrtf(radius / this->radius); // difference between new and old.

	this->radius = radius;

	size_t i, j;
	size_t count = vertices.size();
	for (i = 0; i < count; ++i) {
		this->vertices[i] *= scale;
		this->interleavedVertices[i].Position *= scale;
	}
}

void Sphere::setSectorCount(int sectorCount) {
	this->buildSphere(this->radius, sectorCount, this->stackCount, this->smooth);
}

void Sphere::setStackCount(int stackCount) {
	this->buildSphere(this->radius, this->sectorCount, stackCount, this->smooth);
}

void Sphere::setSmooth(bool smooth) {
	if (this->smooth == smooth) {
		return;
	}
	this->buildSphere(this->radius, this->sectorCount, this->stackCount, smooth);
}

////////////////////////////////////////////////////////////////////////////////////////////
// builders
////////////////////////////////////////////////////////////////////////////////////////////

void Sphere::buildVerticesSmooth() {
	this->clearData();

	float xz_dist, y_dist, lengthInv = 1.0f / radius;
	glm::vec3 pos;
	glm::vec3 normal;
	glm::vec2 texCoord;

	float sectorStep = 2 * M_PI / this->sectorCount;	// space between sectors. sectors wrap around whole cube
	float stackStep = M_PI / this->stackCount;			// space between stacks. stacks only go top to bottom
	float sectorAngle, stackAngle;						// angle between stack and sectors.

	for (int i = 0; i <= this->stackCount; ++i) { 
		stackAngle = M_PI / 2 - i * stackStep;				// stack angle goes from -pi/2 to pi/2.
		float xz_dist = this->radius * cosf(stackAngle);	// r * cos(theta)  this is the distance in the xz plane from the center of the circle
		float y_dist = this->radius * sinf(stackAngle);		// r * sin(phi)	this is the distance in the y direction from the center of the circle

		// add sectorCount + 1 vertices per stack.
		// first and last vertices have same position and normal but different texture coords
		for (int j = 0; j <= this->sectorCount; ++j) {
			sectorAngle = j * sectorStep; // sector angle goes from 0 to 2pi

			pos.x = xz_dist * sinf(sectorAngle);	// x = r * cos(theta) * cos(phi)
			pos.y = y_dist;							// z = r * sin(theta)
			pos.z = xz_dist * cosf(sectorAngle);	// y = r * cos(theta) * sin(phi)
			this->vertices.push_back(pos);

			this->normals.push_back(glm::normalize(pos)); // because circle is centered on origin position is also direction of normal

			texCoord.s = (float)j / (float)this->sectorCount;
			texCoord.t = (float)i / (float)this->stackCount;
			this->texCoords.push_back(texCoord);
		}
	}

	bool oddRow = false;
	for (int i = 0; i < this->stackCount; ++i) {
		if (!oddRow) {
			for (int j = 0; j <= this->sectorCount; ++j) {
				this->indices.push_back(i * (sectorCount + 1) + j);
				this->indices.push_back((i + 1) * (sectorCount + 1) + j);
			}
		}
		else {
			for (int j = this->sectorCount; j >= 0; --j) {
				this->indices.push_back((i + 1) * (sectorCount + 1) + j);
				this->indices.push_back(i * (sectorCount + 1) + j);
			}
		}
	}

	this->buildInterleavedVertices();
}

void Sphere::buildVerticesFlat() {
	struct Vertex
	{
		float x, y, z, s, t;
	};
	std::vector<Vertex> tmpVertices;

	float sectorStep = 2 * M_PI / sectorCount;	// space between sectors. sectors wrap around whole cube
	float stackStep = M_PI / stackCount;		// space between stacks. stacks only go top to bottom
	float sectorAngle, stackAngle;

	for (int i = 0; i <= stackCount; ++i) {
		stackAngle = M_PI / 2 - i * stackStep; // stack angle goes from -pi/2 to pi/2.
		float xz_dist = this->radius * cosf(stackAngle); // r * cos(u)  this is the distance in the xy plane from the center of the circle
		float y_dist = this->radius * sinf(stackAngle); // r * sin(y)	this is the distance in the z direction from the center of the circle

		// add sectorCount + 1 vertices per stack.
		// first and last vertices have same position and normal but different texture coords
		for (int j = 0; j <= sectorCount; ++j) {
			sectorAngle = j * sectorStep; // sector angle goes from 0 to 2pi

			Vertex vertex;
			vertex.x = xz_dist * sinf(sectorAngle);	// x = r * cos(u) * cos(v)
			vertex.y = y_dist;						// y = r * sin(u)
			vertex.z = xz_dist * cosf(sectorAngle);	// z = r * cos(u) * sin(v)
			vertex.s = (float)j / sectorCount;
			vertex.t = (float)i / stackCount;
			tmpVertices.push_back(vertex);
		}
	}

	this->clearData();

	Vertex v1, v2, v3, v4;	// 4 vertices and tex coords for each square.
	glm::vec3 normal;	// normal per face

	int i, j, k, vi1, vi2;
	int index = 0;	// index per vertex

	// iterate over the data, creating triangles by assigning indices to relevent data
	for (i = 0; i < stackCount; ++i) {
		vi1 = i * (sectorCount + 1);		// indices for tmpVertices
		vi2 = (i + 1) * (sectorCount + 1);

		for (j = 0; j < sectorCount; ++j, ++vi1, ++vi2) {
			// 4 vertices per sector
			// v1--v3
			// |    |
			// v2--v4
			v1 = tmpVertices[vi1];
			v2 = tmpVertices[vi2];
			v3 = tmpVertices[vi1 + 1];
			v4 = tmpVertices[vi2 + 1];

			// first stack and last stack are 1 triangle faces
			// otherwise 2 tirangles (quad) per sector
			if (i == 0) { // triangle for first stack uses v1, v2, v4
				// Adding triangle vertices
				this->vertices.push_back(glm::vec3(v1.x, v1.y, v1.z));
				this->vertices.push_back(glm::vec3(v2.x, v2.y, v2.z));
				this->vertices.push_back(glm::vec3(v4.x, v4.y, v4.z));

				// adding texture coords for trinagle vertices
				this->texCoords.push_back(glm::vec2(v1.s, v1.t));
				this->texCoords.push_back(glm::vec2(v2.s, v2.t));
				this->texCoords.push_back(glm::vec2(v4.s, v4.t));

				// adding normals for vertices
				glm::vec3 normal = computeFaceNormal(glm::vec3(v1.x, v1.y, v1.z), glm::vec3(v2.x, v2.y, v2.z), glm::vec3(v4.x, v4.y, v4.z));
				for (k = 0; k < 3; ++k) {
					this->normals.push_back(normal);
				}

				// adding indices for vertices
				this->indices.push_back(index);
				this->indices.push_back(index + 1);
				this->indices.push_back(index + 2);

				// adding indices for lines
				this->lineIndices.push_back(index);
				this->lineIndices.push_back(index + 1);
				index += 3;
			}
			else if (i == (stackCount - 1)) { // triangle for last stack uses v3 instead of v4
				this->vertices.push_back(glm::vec3(v1.x, v1.y, v1.z));
				this->vertices.push_back(glm::vec3(v2.x, v2.y, v2.z));
				this->vertices.push_back(glm::vec3(v3.x, v3.y, v3.z));

				this->texCoords.push_back(glm::vec2(v1.s, v1.t));
				this->texCoords.push_back(glm::vec2(v2.s, v2.t));
				this->texCoords.push_back(glm::vec2(v3.s, v3.t));

				normal = computeFaceNormal(glm::vec3(v1.x, v1.y, v1.z), glm::vec3(v2.x, v2.y, v2.z), glm::vec3(v3.x, v3.y, v3.z));
				for (k = 0; k < 3; ++k) {
					this->normals.push_back(normal);
				}

				this->indices.push_back(index);
				this->indices.push_back(index + 1);
				this->indices.push_back(index + 2);

				// triangle in last stack requires horizontal lines as well as vertical 
				this->lineIndices.push_back(index);
				this->lineIndices.push_back(index + 1);
				this->lineIndices.push_back(index);
				this->lineIndices.push_back(index + 2);

				index += 3;
			}
			else { // triangle for all other stacks use all vertices
				this->vertices.push_back(glm::vec3(v1.x, v1.y, v1.z));
				this->vertices.push_back(glm::vec3(v2.x, v2.y, v2.z));
				this->vertices.push_back(glm::vec3(v3.x, v3.y, v3.z));
				this->vertices.push_back(glm::vec3(v4.x, v4.y, v4.z));

				this->texCoords.push_back(glm::vec2(v1.s, v1.t));
				this->texCoords.push_back(glm::vec2(v2.s, v2.t));
				this->texCoords.push_back(glm::vec2(v3.s, v3.t));
				this->texCoords.push_back(glm::vec2(v4.s, v4.t));

				glm::vec3 normal = computeFaceNormal(glm::vec3(v1.x, v1.y, v1.z), glm::vec3(v2.x, v2.y, v2.z), glm::vec3(v3.x, v3.y, v3.z));
				for (k = 0; k < 4; ++k) {
					this->normals.push_back(normal);
				}

				// first triangle
				this->indices.push_back(index);
				this->indices.push_back(index + 1);
				this->indices.push_back(index + 2);
				// second triangle
				this->indices.push_back(index + 2);
				this->indices.push_back(index + 1);
				this->indices.push_back(index + 3);

				// indices for horizontal and vertical lines
				this->lineIndices.push_back(index);
				this->lineIndices.push_back(index + 1);
				this->lineIndices.push_back(index);
				this->lineIndices.push_back(index + 2);

				index += 4;
			}
		}
	}
	this->buildInterleavedVertices();
}

void Sphere::buildInterleavedVertices() {
	std::vector<VertexData>().swap(this->interleavedVertices);

	std::size_t i;
	std::size_t count = vertices.size();
	for (i = 0; i < count; ++i) {
		VertexData vd;

		vd.Position = glm::vec3(this->vertices[i]);
		vd.Normal = glm::vec3(this->normals[i]);
		vd.TexCoord = glm::vec2(this->texCoords[i]);

		this->interleavedVertices.push_back(vd);
	}
}

///////////////////////////////////////////////////////////////////////////////
// return face normal of a triangle v1-v2-v3
// if a triangle has no surface (normal length = 0), then return a zero vector
///////////////////////////////////////////////////////////////////////////////
glm::vec3 Sphere::computeFaceNormal(glm::vec3 v1,
									glm::vec3 v2,
									glm::vec3 v3)
{
    const float EPSILON = 0.000001f;

    glm::vec3 normal = glm::vec3(0.0f);     // default return value (0,0,0)
    float nx, ny, nz;

    // find 2 edge vectors: v1-v2, v1-v3
	glm::vec3 edge1 = glm::vec3(
		v2.x - v1.x, 
		v2.y - v1.y, 
		v2.z - v1.z
	);
	glm::vec3 edge2 = glm::vec3(
		v3.x - v1.x, 
		v3.y - v1.y, 
		v3.z - v1.z
	);

    // cross product: e1 x e2
	normal = glm::cross(edge1, edge2);

	if (glm::length(normal) > EPSILON) {
		normal = glm::normalize(normal);
	}

    return normal;
}

void Sphere::clearData() {
	std::vector<unsigned int>().swap(this->indices);
	std::vector<unsigned int>().swap(this->lineIndices);
	std::vector<glm::vec3>().swap(this->vertices);
	std::vector<glm::vec2>().swap(this->texCoords);
	std::vector<glm::vec3>().swap(this->normals);
	if (this->VAO != 0)
		glDeleteBuffers(1, &this->VAO);
}

////////////////////////////////////////////////////////////////////////////////////////////
// drawers
////////////////////////////////////////////////////////////////////////////////////////////

void Sphere::genVAO() {
	glGenVertexArrays(1, &VAO);
	glBindVertexArray(VAO);

	// load data into vertex buffers
	// copy vertex buffer data
	glGenBuffers(1, &VBO);
	glBindBuffer(GL_ARRAY_BUFFER, VBO);
	glBufferData(GL_ARRAY_BUFFER, this->getInterleavedVertexSize(), this->getInterleavedVertices(), GL_STATIC_DRAW);

	// copy index buffer data
	glGenBuffers(1, &EBO);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, this->getIndexSize(), this->getIndices(), GL_STATIC_DRAW);
	//checkGLError("setupMesh buffers");

	// set the vertex attribute pointers for shader
	glEnableVertexAttribArray(0); // vertex positions
	glEnableVertexAttribArray(1); // vertex normals	
	glEnableVertexAttribArray(2); // vertex texCoords
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(VertexData), (void*)0);
	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(VertexData), (void*)offsetof(VertexData, Normal));
	glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, sizeof(VertexData), (void*)offsetof(VertexData, TexCoord));

	//checkGLError("setupMesh attribs");
	// cleanup
	glBindVertexArray(0);
	glDeleteBuffers(1, &VBO);
	glDeleteBuffers(1, &EBO);
}

void Sphere::Draw(const Shader& shader, GLuint baseUnit) {
	if (this->VAO == 0) {
		this->genVAO();
	}

	shader.setVec4("material.ambient", this->material->AmbientColor);
	shader.setVec4("material.diffuse", this->material->DiffuseColor);
	shader.setVec4("material.specular", this->material->SpecularColor);
	shader.setFloat("material.shininess", this->material->Shininess);
	shader.setFloat("material.opacity", this->material->Opacity);
	shader.setFloat("material.reflectivity", this->material->Reflectivity);
	shader.setFloat("material.refractionIndex", this->material->RefractionIndex);

	glBindVertexArray(VAO);
	glDrawElements(this->smooth ? GL_TRIANGLE_STRIP : GL_TRIANGLES, indices.size(), GL_UNSIGNED_INT, 0);
}

void Sphere::genLineVAO() {
	glGenVertexArrays(1, &this->lineVAO);
	glBindVertexArray(this->lineVAO);

	// load data into vertex buffers
	// copy vertex buffer data
	glGenBuffers(1, &this->lineVBO);
	glBindBuffer(GL_ARRAY_BUFFER, this->lineVBO);
	glBufferData(GL_ARRAY_BUFFER, this->getVertexSize(), this->getVertices(), GL_STATIC_DRAW);

	// copy index buffer data
	glGenBuffers(1, &this->lineEBO);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, this->lineEBO);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, this->getLineIndexSize(), this->getLineIndices(), GL_STATIC_DRAW);
	//checkGLError("setupMesh buffers");

	// set the vertex attribute pointers for shader
	glEnableVertexAttribArray(0); // vertex positions
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(glm::vec3), (void*)0);

	//checkGLError("setupMesh attribs");
	// cleanup
	glBindVertexArray(0);
	glDeleteBuffers(1, &this->lineVBO);
	glDeleteBuffers(1, &this->lineEBO);
}

void Sphere::drawLines() {
	if (this->lineVAO == 0) {
		this->genLineVAO();
	}
	glBindVertexArray(lineVAO);
	glDrawElements(GL_LINES, (unsigned int)lineIndices.size(), GL_UNSIGNED_INT, lineIndices.data());
}
