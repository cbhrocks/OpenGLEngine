#include "Icosphere.h"

Icosphere::Icosphere(std::string name, float radius, int subdivisions, bool smooth) : IDrawObj(name) {
	this->buildIcosphere(radius, subdivisions, smooth);
}

void Icosphere::buildIcosphere(float radius, int subdivisions, bool smooth) {
	this->radius = radius;
	this->subdivisions = subdivisions;

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

void Icosphere::setRadius(float radius) {
	float scale = sqrtf(radius / this->radius); // difference between new and old.

	this->radius = radius;

	size_t i, j;
	size_t count = vertices.size();
	for (i = 0; i < count; ++i) {
		this->vertices[i] *= scale;
		this->interleavedVertices[i].Position *= scale;
	}
}

void Icosphere::setSubdivisions(int subdivisions) {
	this->buildIcosphere(this->radius, subdivisions, this->smooth);
}

void Icosphere::setSmooth(bool smooth) {
	if (this->smooth == smooth) {
		return;
	}
	this->buildIcosphere(this->radius, this->subdivisions, smooth);
}

////////////////////////////////////////////////////////////////////////////////////////////
// builders
////////////////////////////////////////////////////////////////////////////////////////////

void Icosphere::buildVerticesSmooth() {
	this->clearData();

	float latitude = atanf(0.5);
	float longitude_step = M_PI * 2 / 10;

	float xz_dist = cosf(latitude) * this->radius;
	float y_dist = sinf(latitude) * this->radius;

	unsigned int index = 0;

	glm::vec3 pos = glm::vec3(0.0f, 1.0f, 0.0f) * this->radius;
	for (int i = 0; i < 5; i++) {
		this->vertices.push_back(pos); // top
		this->vertexLoc.insert(std::make_pair(pos, index++));
		this->normals.push_back(glm::normalize(pos));
		this->texCoords.push_back(glm::vec2((0.5 + i)/5.5f, 0.0f));
	}

	bool even = true;
	for (int i = 0; i < 6; i++) {
		pos.x = xz_dist * cosf(2 * i * longitude_step);	// x = r * cos(theta) * sin(phi)
		pos.y = y_dist;		// y = r * sin(theta)
		pos.z = xz_dist * sinf(2 * i * longitude_step);	// z = r * cos(theta) * cos(phi)
		this->vertices.push_back(pos);
		this->vertexLoc.insert(std::make_pair(pos, index++));
		this->normals.push_back(glm::normalize(pos));
		this->texCoords.push_back(glm::vec2((float)(0 + i) / 5.5f, 1.0f / 3.0f));
	}

	for (int i = 0; i < 6; i++) {
		pos.x = xz_dist * cosf(longitude_step + 2 * i * longitude_step);	// x = r * cos(theta) * sin(phi)
		pos.y = -y_dist;		// y = r * sin(theta)
		pos.z = xz_dist * sinf(longitude_step + 2 * i * longitude_step);	// z = r * cos(theta) * cos(phi)
		this->vertices.push_back(pos);
		this->vertexLoc.insert(std::make_pair(pos, index++));
		this->normals.push_back(glm::normalize(pos));
		this->texCoords.push_back(glm::vec2((float)(0.5 + i) / 5.5f, 2.0f / 3.0f));
	}

	pos = glm::vec3(0.0f, -1.0f, 0.0f) * this->radius;
	for (int i = 0; i < 5; i++) {
		this->vertices.push_back(pos); // top
		this->vertexLoc.insert(std::make_pair(pos, index++));
		this->normals.push_back(glm::normalize(pos));
		this->texCoords.push_back(glm::vec2((1.5 + i)/5.5, 1.0f));
	}

	// working
	this->triangles = {
		// first row
		Triangle(0,6,5),
		Triangle(1,7,6),
		Triangle(2,8,7),
		Triangle(3,9,8),
		Triangle(4,10,9),
		// second row
		Triangle(5,6,11),
		Triangle(6,7,12),
		Triangle(7,8,13),
		Triangle(8,9,14),
		Triangle(9,10,15),
		// third row
		Triangle(6,12,11),
		Triangle(7,13,12),
		Triangle(8,14,13),
		Triangle(9,15,14),
		Triangle(10,16,15),
		// last row 
		Triangle(11,12,17),
		Triangle(12,13,18),
		Triangle(13,14,19),
		Triangle(14,15,20),
		Triangle(15,16,21)
	};

	for (int i = 0; i < this->subdivisions; i++) {
		this->subdivide();
	}
	this->buildInterleavedVertices();
	this->buildIndices();
}

void Icosphere::buildVerticesFlat() {
	this->clearData();

	float latitude = atanf(0.5);
	float longitude_step = M_PI * 2 / 5;

	float xz_dist = cosf(latitude) * this->radius;
	float y_dist = sinf(latitude) * this->radius;

	int index = 0;
	for (int i = 0; i < 5; i++) {
		//top vertex
		glm::vec3 v1 = glm::vec3(0.0f, 1.0f, 0.0f) * this->radius;
		//left vertex
		glm::vec3 v2 = glm::vec3(
			xz_dist * cosf(i * longitude_step),	// x = r * cos(theta) * sin(phi)
			y_dist,		// y = r * sin(theta)
			xz_dist * sinf(i * longitude_step)	// z = r * cos(theta) * cos(phi)
		);
		// right vertex
		glm::vec3 v3 = glm::vec3(
			xz_dist * cosf((i + 1) * longitude_step),	// x = r * cos(theta) * sin(phi)
			y_dist,		// y = r * sin(theta)
			xz_dist * sinf((i + 1) * longitude_step)	// z = r * cos(theta) * cos(phi)
		);

		glm::vec3 normal = glm::cross(v3 - v1, v2 - v1);
		this->vertices.push_back(v1);
		this->vertices.push_back(v3);
		this->vertices.push_back(v2);
		this->normals.push_back(normal);
		this->normals.push_back(normal);
		this->normals.push_back(normal);
		this->texCoords.push_back(glm::vec2((float)(0.5 + i)/5.5f, 0.0f));
		this->texCoords.push_back(glm::vec2((float)(0 + i + 1) / 5.5f, 1.0f / 3.0f));
		this->texCoords.push_back(glm::vec2((float)(0 + i) / 5.5f, 1.0f / 3.0f));

		this->triangles.push_back(Triangle(index, index + 1, index + 2));

		//bottom vertex
		glm::vec3 v4 = glm::vec3(
			xz_dist * cosf(longitude_step/2 + i * longitude_step),	// x = r * cos(theta) * sin(phi)
			- y_dist,		// y = r * sin(theta)
			xz_dist * sinf(longitude_step/2 + i * longitude_step)	// z = r * cos(theta) * cos(phi)
		);

		normal = glm::cross(v4 - v2, v4 - v3);
		this->vertices.push_back(v2);
		this->vertices.push_back(v3);
		this->vertices.push_back(v4);
		this->normals.push_back(normal);
		this->normals.push_back(normal);
		this->normals.push_back(normal);
		this->texCoords.push_back(glm::vec2((float)(0 + i) / 5.5f, 1.0f / 3.0f));
		this->texCoords.push_back(glm::vec2((float)(0 + i + 1) / 5.5f, 1.0f / 3.0f));
		this->texCoords.push_back(glm::vec2((float)(0.5 + i) / 5.5f, 2.0f / 3.0f));

		this->triangles.push_back(Triangle(index + 3, index + 4, index + 5));
		index += 6;
	}

	for (int i = 0; i < 5; i++) {
		//top vertex
		glm::vec3 v1 = glm::vec3(
			xz_dist * cosf(longitude_step + i * longitude_step),	// x = r * cos(theta) * sin(phi)
			y_dist,		// y = r * sin(theta)
			xz_dist * sinf(longitude_step + i * longitude_step)	// z = r * cos(theta) * cos(phi)
		);
		//left vertex
		glm::vec3 v2 = glm::vec3(
			xz_dist * cosf(longitude_step/2 + i * longitude_step),	// x = r * cos(theta) * sin(phi)
			-y_dist,		// y = r * sin(theta)
			xz_dist * sinf(longitude_step/2 + i * longitude_step)	// z = r * cos(theta) * cos(phi)
		);
		// right vertex
		glm::vec3 v3 = glm::vec3(
			xz_dist * cosf(longitude_step/2 + (i + 1) * longitude_step),	// x = r * cos(theta) * sin(phi)
			-y_dist,		// y = r * sin(theta)
			xz_dist * sinf(longitude_step/2 + (i + 1) * longitude_step)	// z = r * cos(theta) * cos(phi)
		);

		glm::vec3 normal = glm::cross(v3 - v1, v2 - v1);
		this->vertices.push_back(v1);
		this->vertices.push_back(v3);
		this->vertices.push_back(v2);
		this->normals.push_back(normal);
		this->normals.push_back(normal);
		this->normals.push_back(normal);
		this->texCoords.push_back(glm::vec2((float)(i + 1) / 5.5f, 1.0f / 3.0f));
		this->texCoords.push_back(glm::vec2((float)(i + 1.5) / 5.5f, 2.0f / 3.0f));
		this->texCoords.push_back(glm::vec2((float)(i + 0.5) / 5.5f, 2.0f / 3.0f));

		this->triangles.push_back(Triangle(index, index + 1, index + 2));

		//bottom vertex
		glm::vec3 v4 = glm::vec3(0.0f, -1.0f, 0.0f) * this->radius;

		normal = glm::cross(v4 - v2, v4 - v3);
		this->vertices.push_back(v2);
		this->vertices.push_back(v3);
		this->vertices.push_back(v4);
		this->normals.push_back(normal);
		this->normals.push_back(normal);
		this->normals.push_back(normal);
		this->texCoords.push_back(glm::vec2((float)(i + 0.5f) / 5.5f, 2.0f / 3.0f));
		this->texCoords.push_back(glm::vec2((float)(i + 1.5f) / 5.5f, 2.0f / 3.0f));
		this->texCoords.push_back(glm::vec2((float)(1.5f + i)/5.5f, 1.0f));

		this->triangles.push_back(Triangle(index + 3, index + 4, index + 5));
		index += 6;
	}

	for (int i = 0; i < this->subdivisions; i++) {
		this->subdivide();
	}
	this->buildInterleavedVertices();
	this->buildIndices();
}

void Icosphere::subdivide() {
	int triangleNum = this->triangles.size();
	for (int j = 0; j < triangleNum; j++) {
		if (this->smooth == true) {
			this->subdivideTriangleSmooth(j);
		}
		else {
			this->subdivideTriangleFlat(j);
		}
	}
}

void Icosphere::subdivideTriangleSmooth(unsigned int index) {
	Triangle tri = this->triangles.at(index);
	glm::vec3 v1, v2, v3;
	v1 = this->vertices.at(tri.i1);
	v2 = this->vertices.at(tri.i2);
	v3 = this->vertices.at(tri.i3);
	glm::vec3 n1, n2, n3;
	n1 = this->normals.at(tri.i1);
	n2 = this->normals.at(tri.i2);
	n3 = this->normals.at(tri.i3);
	glm::vec2 t1, t2, t3;
	t1 = this->texCoords.at(tri.i1);
	t2 = this->texCoords.at(tri.i2);
	t3 = this->texCoords.at(tri.i3);

	glm::vec3 v12, v13, v23;
	v12 = glm::normalize((v1 + v2) * 0.5f) * this->radius;
	v13 = glm::normalize((v1 + v3) * 0.5f) * this->radius;
	v23 = glm::normalize((v2 + v3) * 0.5f) * this->radius;
	glm::vec3 n12, n13, n23;
	n12 = glm::normalize((n1 + n2) * 0.5f) * this->radius;
	n13 = glm::normalize((n1 + n3) * 0.5f) * this->radius;
	n23 = glm::normalize((n2 + n3) * 0.5f) * this->radius;
	glm::vec2 t12, t13, t23;
	t12 = (t1 + t2) * 0.5f;
	t13 = (t1 + t3) * 0.5f;
	t23 = (t2 + t3) * 0.5f;


	unsigned int i12, i13, i23;
	// make sure that the triangle's vertex isn't already stored.
	// if not add it, if is set to existing index
	if (vertexLoc.count(v12) > 0) {
		i12 = vertexLoc.at(v12);
	}
	else {
		this->vertices.push_back(v12);
		this->normals.push_back(glm::normalize(v12));
		this->texCoords.push_back(t12);
		i12 = this->vertices.size() - 1;
		this->vertexLoc.insert(std::make_pair(v12, i12));
	}
	if (vertexLoc.count(v13) > 0) {
		i13 = vertexLoc.at(v13);
	}
	else {
		this->vertices.push_back(v13);
		this->normals.push_back(glm::normalize(v13));
		this->texCoords.push_back(t13);
		i13 = this->vertices.size() - 1;
		this->vertexLoc.insert(std::make_pair(v13, i13));
	}
	if (vertexLoc.count(v23) > 0) {
		i23 = vertexLoc.at(v23);
	}
	else {
		this->vertices.push_back(v23);
		this->normals.push_back(glm::normalize(v23));
		this->texCoords.push_back(t23);
		i23 = this->vertices.size() - 1;
		this->vertexLoc.insert(std::make_pair(v23, i23));
	}
	this->triangles.push_back(Triangle(tri.i1, i12, i13));
	this->triangles.push_back(Triangle(i12, tri.i2, i23));
	this->triangles.push_back(Triangle(i23, tri.i3, i13));
	this->triangles[index] = Triangle(i12, i23, i13);
}

void Icosphere::subdivideTriangleFlat(unsigned int index) {
	Triangle tri = this->triangles.at(index);
	glm::vec3 v1, v2, v3;
	v1 = this->vertices.at(tri.i1);
	v2 = this->vertices.at(tri.i2);
	v3 = this->vertices.at(tri.i3);
	glm::vec3 n1, n2, n3;
	n1 = this->normals.at(tri.i1);
	n2 = this->normals.at(tri.i2);
	n3 = this->normals.at(tri.i3);
	glm::vec2 t1, t2, t3;
	t1 = this->texCoords.at(tri.i1);
	t2 = this->texCoords.at(tri.i2);
	t3 = this->texCoords.at(tri.i3);

	glm::vec3 v12, v13, v23;
	v12 = glm::normalize((v1 + v2) * 0.5f) * this->radius;
	v13 = glm::normalize((v1 + v3) * 0.5f) * this->radius;
	v23 = glm::normalize((v2 + v3) * 0.5f) * this->radius;
	glm::vec2 t12, t13, t23;
	t12 = (t1 + t2) * 0.5f;
	t13 = (t1 + t3) * 0.5f;
	t23 = (t2 + t3) * 0.5f;
	glm::vec3 normal;

	// new triangle 1
	this->vertices.push_back(v1); // top
	this->vertices.push_back(v12); // left
	this->vertices.push_back(v13); // right
	normal = glm::normalize(glm::cross(v12 - v1, v13 - v1));
	this->normals.push_back(normal);
	this->normals.push_back(normal);
	this->normals.push_back(normal);
	this->texCoords.push_back(t1);
	this->texCoords.push_back(t12);
	this->texCoords.push_back(t13);
	this->triangles.push_back(Triangle(vertices.size() - 3, vertices.size() - 2, vertices.size() - 1));
	// new triangle 2
	this->vertices.push_back(v12); // top
	this->vertices.push_back(v2); // left
	this->vertices.push_back(v23); // right
	normal = glm::normalize(glm::cross(v2 - v12, v23 - v12));
	this->normals.push_back(normal);
	this->normals.push_back(normal);
	this->normals.push_back(normal);
	this->texCoords.push_back(t12);
	this->texCoords.push_back(t2);
	this->texCoords.push_back(t23);
	this->triangles.push_back(Triangle(vertices.size() - 3, vertices.size() - 2, vertices.size() - 1));
	// new triangle 3
	this->vertices.push_back(v13); // top
	this->vertices.push_back(v23); // left
	this->vertices.push_back(v3); // right
	normal = glm::normalize(glm::cross(v23 - v13, v3 - v13));
	this->normals.push_back(normal);
	this->normals.push_back(normal);
	this->normals.push_back(normal);
	this->texCoords.push_back(t12);
	this->texCoords.push_back(t23);
	this->texCoords.push_back(t3);
	this->triangles.push_back(Triangle(vertices.size() - 3, vertices.size() - 2, vertices.size() - 1));
	// middle triangle
	this->vertices[tri.i1] = v12; // top
	this->vertices[tri.i2] = v23; // left
	this->vertices[tri.i3] = v13; // right
	normal = glm::normalize(glm::cross(v13 - v23, v12 - v23));
	this->normals[tri.i1] = normal;
	this->normals[tri.i2] = normal;
	this->normals[tri.i3] = normal;
	this->texCoords[tri.i1] = t23;
	this->texCoords[tri.i2] = t12;
	this->texCoords[tri.i3] = t13;
}

void Icosphere::buildInterleavedVertices() {
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

void Icosphere::buildIndices() {
	std::vector<unsigned int>().swap(this->indices);
	for (Triangle triangle : this->triangles) {
		this->indices.push_back(triangle.i1);
		this->indices.push_back(triangle.i2);
		this->indices.push_back(triangle.i3);
	}
}

///////////////////////////////////////////////////////////////////////////////
// return face normal of a triangle v1-v2-v3
// if a triangle has no surface (normal length = 0), then return a zero vector
///////////////////////////////////////////////////////////////////////////////
glm::vec3 Icosphere::computeFaceNormal(glm::vec3 v1,
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

void Icosphere::clearData() {
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

void Icosphere::genVAO() {
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

void Icosphere::Draw(const Shader& shader, GLuint baseUnit) {
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
	glDrawElements(GL_TRIANGLES, indices.size(), GL_UNSIGNED_INT, 0);
}

void Icosphere::genLineVAO() {
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

void Icosphere::drawLines() {
	if (this->lineVAO == 0) {
		this->genLineVAO();
	}
	glBindVertexArray(lineVAO);
	glDrawElements(GL_LINES, (unsigned int)lineIndices.size(), GL_UNSIGNED_INT, lineIndices.data());
}
