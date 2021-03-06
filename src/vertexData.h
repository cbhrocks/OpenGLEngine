#pragma once

#include <glad/glad.h> // holds all OpenGL type declarations
#include <glm/glm.hpp>
#include <vector>
#include <algorithm>

// A great thing about structs is that their memory layout is sequential for all its items.
// The effect is that we can simply pass a pointer to the struct and it translates perfectly to a glm::vec3/2 array which
// again translates to 3/2 floats which translates to a byte array.
struct VertexData {
    // position
    glm::vec3 Position;
    // normal
    glm::vec3 Normal;
    // texCoords
    glm::vec2 uv;
    // tangent
    glm::vec3 Tangent;
    // bitangent
    glm::vec3 Bitangent;
};

GLfloat const skyboxVertexData[] = {
    // positions          
    -1.0f,  1.0f, -1.0f,
    -1.0f, -1.0f, -1.0f,
     1.0f, -1.0f, -1.0f,
     1.0f, -1.0f, -1.0f,
     1.0f,  1.0f, -1.0f,
    -1.0f,  1.0f, -1.0f,

    -1.0f, -1.0f,  1.0f,
    -1.0f, -1.0f, -1.0f,
    -1.0f,  1.0f, -1.0f,
    -1.0f,  1.0f, -1.0f,
    -1.0f,  1.0f,  1.0f,
    -1.0f, -1.0f,  1.0f,

     1.0f, -1.0f, -1.0f,
     1.0f, -1.0f,  1.0f,
     1.0f,  1.0f,  1.0f,
     1.0f,  1.0f,  1.0f,
     1.0f,  1.0f, -1.0f,
     1.0f, -1.0f, -1.0f,

    -1.0f, -1.0f,  1.0f,
    -1.0f,  1.0f,  1.0f,
     1.0f,  1.0f,  1.0f,
     1.0f,  1.0f,  1.0f,
     1.0f, -1.0f,  1.0f,
    -1.0f, -1.0f,  1.0f,

    -1.0f,  1.0f, -1.0f,
     1.0f,  1.0f, -1.0f,
     1.0f,  1.0f,  1.0f,
     1.0f,  1.0f,  1.0f,
    -1.0f,  1.0f,  1.0f,
    -1.0f,  1.0f, -1.0f,

    -1.0f, -1.0f, -1.0f,
    -1.0f, -1.0f,  1.0f,
     1.0f, -1.0f, -1.0f,
     1.0f, -1.0f, -1.0f,
    -1.0f, -1.0f,  1.0f,
     1.0f, -1.0f,  1.0f
};


GLfloat const textureCube[] = {
    // Back face
    -0.5f, -0.5f, -0.5f,  0.0f, 0.0f, // Bottom-left
     0.5f,  0.5f, -0.5f,  1.0f, 1.0f, // top-right
     0.5f, -0.5f, -0.5f,  1.0f, 0.0f, // bottom-right         
     0.5f,  0.5f, -0.5f,  1.0f, 1.0f, // top-right
    -0.5f, -0.5f, -0.5f,  0.0f, 0.0f, // bottom-left
    -0.5f,  0.5f, -0.5f,  0.0f, 1.0f, // top-left
    // Front face
    -0.5f, -0.5f,  0.5f,  0.0f, 0.0f, // bottom-left
     0.5f, -0.5f,  0.5f,  1.0f, 0.0f, // bottom-right
     0.5f,  0.5f,  0.5f,  1.0f, 1.0f, // top-right
     0.5f,  0.5f,  0.5f,  1.0f, 1.0f, // top-right
    -0.5f,  0.5f,  0.5f,  0.0f, 1.0f, // top-left
    -0.5f, -0.5f,  0.5f,  0.0f, 0.0f, // bottom-left
    // Left face
    -0.5f,  0.5f,  0.5f,  1.0f, 0.0f, // top-right
    -0.5f,  0.5f, -0.5f,  1.0f, 1.0f, // top-left
    -0.5f, -0.5f, -0.5f,  0.0f, 1.0f, // bottom-left
    -0.5f, -0.5f, -0.5f,  0.0f, 1.0f, // bottom-left
    -0.5f, -0.5f,  0.5f,  0.0f, 0.0f, // bottom-right
    -0.5f,  0.5f,  0.5f,  1.0f, 0.0f, // top-right
    // Right face
     0.5f,  0.5f,  0.5f,  1.0f, 0.0f, // top-left
     0.5f, -0.5f, -0.5f,  0.0f, 1.0f, // bottom-right
     0.5f,  0.5f, -0.5f,  1.0f, 1.0f, // top-right         
     0.5f, -0.5f, -0.5f,  0.0f, 1.0f, // bottom-right
     0.5f,  0.5f,  0.5f,  1.0f, 0.0f, // top-left
     0.5f, -0.5f,  0.5f,  0.0f, 0.0f, // bottom-left     
    // Bottom face
    -0.5f, -0.5f, -0.5f,  0.0f, 1.0f, // top-right
     0.5f, -0.5f, -0.5f,  1.0f, 1.0f, // top-left
     0.5f, -0.5f,  0.5f,  1.0f, 0.0f, // bottom-left
     0.5f, -0.5f,  0.5f,  1.0f, 0.0f, // bottom-left
    -0.5f, -0.5f,  0.5f,  0.0f, 0.0f, // bottom-right
    -0.5f, -0.5f, -0.5f,  0.0f, 1.0f, // top-right
    // Top face
    -0.5f,  0.5f, -0.5f,  0.0f, 1.0f, // top-left
     0.5f,  0.5f,  0.5f,  1.0f, 0.0f, // bottom-right
     0.5f,  0.5f, -0.5f,  1.0f, 1.0f, // top-right     
     0.5f,  0.5f,  0.5f,  1.0f, 0.0f, // bottom-right
    -0.5f,  0.5f, -0.5f,  0.0f, 1.0f, // top-left
    -0.5f,  0.5f,  0.5f,  0.0f, 0.0f  // bottom-left        
};

GLfloat const normalCube[] = {
    // Back face
    -0.5f, -0.5f, -0.5f,  0.0f,  0.0f, -1.0f,
     0.5f,  0.5f, -0.5f,  0.0f,  0.0f, -1.0f, 
     0.5f, -0.5f, -0.5f,  0.0f,  0.0f, -1.0f, 
     0.5f,  0.5f, -0.5f,  0.0f,  0.0f, -1.0f, 
    -0.5f, -0.5f, -0.5f,  0.0f,  0.0f, -1.0f, 
    -0.5f,  0.5f, -0.5f,  0.0f,  0.0f, -1.0f, 
    // Front face
    -0.5f, -0.5f,  0.5f,  0.0f,  0.0f, 1.0f,
     0.5f, -0.5f,  0.5f,  0.0f,  0.0f, 1.0f,
     0.5f,  0.5f,  0.5f,  0.0f,  0.0f, 1.0f,
     0.5f,  0.5f,  0.5f,  0.0f,  0.0f, 1.0f,
    -0.5f,  0.5f,  0.5f,  0.0f,  0.0f, 1.0f,
    -0.5f, -0.5f,  0.5f,  0.0f,  0.0f, 1.0f,
    // Left face
    -0.5f,  0.5f,  0.5f, -1.0f,  0.0f,  0.0f,
    -0.5f,  0.5f, -0.5f, -1.0f,  0.0f,  0.0f,
    -0.5f, -0.5f, -0.5f, -1.0f,  0.0f,  0.0f,
    -0.5f, -0.5f, -0.5f, -1.0f,  0.0f,  0.0f,
    -0.5f, -0.5f,  0.5f, -1.0f,  0.0f,  0.0f,
    -0.5f,  0.5f,  0.5f, -1.0f,  0.0f,  0.0f,
    // Right face
     0.5f,  0.5f,  0.5f,  1.0f,  0.0f,  0.0f,
     0.5f, -0.5f, -0.5f,  1.0f,  0.0f,  0.0f,
     0.5f,  0.5f, -0.5f,  1.0f,  0.0f,  0.0f,
     0.5f, -0.5f, -0.5f,  1.0f,  0.0f,  0.0f,
     0.5f,  0.5f,  0.5f,  1.0f,  0.0f,  0.0f,
     0.5f, -0.5f,  0.5f,  1.0f,  0.0f,  0.0f,
    // Bottom face
    -0.5f, -0.5f, -0.5f,  0.0f, -1.0f,  0.0f,
     0.5f, -0.5f, -0.5f,  0.0f, -1.0f,  0.0f,
     0.5f, -0.5f,  0.5f,  0.0f, -1.0f,  0.0f,
     0.5f, -0.5f,  0.5f,  0.0f, -1.0f,  0.0f,
    -0.5f, -0.5f,  0.5f,  0.0f, -1.0f,  0.0f,
    -0.5f, -0.5f, -0.5f,  0.0f, -1.0f,  0.0f,
    // Top face
    -0.5f,  0.5f, -0.5f,  0.0f,  1.0f,  0.0f,
     0.5f,  0.5f,  0.5f,  0.0f,  1.0f,  0.0f,
     0.5f,  0.5f, -0.5f,  0.0f,  1.0f,  0.0f,
     0.5f,  0.5f,  0.5f,  0.0f,  1.0f,  0.0f,
    -0.5f,  0.5f, -0.5f,  0.0f,  1.0f,  0.0f,
    -0.5f,  0.5f,  0.5f,  0.0f,  1.0f,  0.0f
};

///<summary>Cube vertex data that includes position, texture coords, and normals.</summary>
GLfloat const cubeVertexData[] = {
	-0.5f, -0.5f, -0.5f,  0.0f,  0.0f, -1.0f,  0.0f, 0.0f,
	 0.5f,  0.5f, -0.5f,  0.0f,  0.0f, -1.0f,  1.0f, 1.0f,
	 0.5f, -0.5f, -0.5f,  0.0f,  0.0f, -1.0f,  1.0f, 0.0f,
	 0.5f,  0.5f, -0.5f,  0.0f,  0.0f, -1.0f,  1.0f, 1.0f,
	-0.5f, -0.5f, -0.5f,  0.0f,  0.0f, -1.0f,  0.0f, 0.0f,
	-0.5f,  0.5f, -0.5f,  0.0f,  0.0f, -1.0f,  0.0f, 1.0f,
 // Front face                     
	-0.5f, -0.5f,  0.5f,  0.0f,  0.0f,  1.0f,  0.0f, 0.0f,
	 0.5f, -0.5f,  0.5f,  0.0f,  0.0f,  1.0f,  1.0f, 0.0f,
	 0.5f,  0.5f,  0.5f,  0.0f,  0.0f,  1.0f,  1.0f, 1.0f,
	 0.5f,  0.5f,  0.5f,  0.0f,  0.0f,  1.0f,  1.0f, 1.0f,
	-0.5f,  0.5f,  0.5f,  0.0f,  0.0f,  1.0f,  0.0f, 1.0f,
	-0.5f, -0.5f,  0.5f,  0.0f,  0.0f,  1.0f,  0.0f, 0.0f,
 // Left face                     
	-0.5f,  0.5f,  0.5f, -1.0f,  0.0f,  0.0f,  1.0f, 0.0f,
	-0.5f,  0.5f, -0.5f, -1.0f,  0.0f,  0.0f,  1.0f, 1.0f,
	-0.5f, -0.5f, -0.5f, -1.0f,  0.0f,  0.0f,  0.0f, 1.0f,
	-0.5f, -0.5f, -0.5f, -1.0f,  0.0f,  0.0f,  0.0f, 1.0f,
	-0.5f, -0.5f,  0.5f, -1.0f,  0.0f,  0.0f,  0.0f, 0.0f,
	-0.5f,  0.5f,  0.5f, -1.0f,  0.0f,  0.0f,  1.0f, 0.0f,
 // Right face                     
	 0.5f,  0.5f,  0.5f,  1.0f,  0.0f,  0.0f,  1.0f, 0.0f,
	 0.5f, -0.5f, -0.5f,  1.0f,  0.0f,  0.0f,  0.0f, 1.0f,
	 0.5f,  0.5f, -0.5f,  1.0f,  0.0f,  0.0f,  1.0f, 1.0f,
	 0.5f, -0.5f, -0.5f,  1.0f,  0.0f,  0.0f,  0.0f, 1.0f,
	 0.5f,  0.5f,  0.5f,  1.0f,  0.0f,  0.0f,  1.0f, 0.0f,
	 0.5f, -0.5f,  0.5f,  1.0f,  0.0f,  0.0f,  0.0f, 0.0f,
 // Bottom face                     
	-0.5f, -0.5f, -0.5f,  0.0f, -1.0f,  0.0f,  0.0f, 1.0f,
	 0.5f, -0.5f, -0.5f,  0.0f, -1.0f,  0.0f,  1.0f, 1.0f,
	 0.5f, -0.5f,  0.5f,  0.0f, -1.0f,  0.0f,  1.0f, 0.0f,
	 0.5f, -0.5f,  0.5f,  0.0f, -1.0f,  0.0f,  1.0f, 0.0f,
	-0.5f, -0.5f,  0.5f,  0.0f, -1.0f,  0.0f,  0.0f, 0.0f,
	-0.5f, -0.5f, -0.5f,  0.0f, -1.0f,  0.0f,  0.0f, 1.0f,
 // Top face                     
	-0.5f,  0.5f, -0.5f,  0.0f,  1.0f,  0.0f,  0.0f, 1.0f,
	 0.5f,  0.5f,  0.5f,  0.0f,  1.0f,  0.0f,  1.0f, 0.0f,
	 0.5f,  0.5f, -0.5f,  0.0f,  1.0f,  0.0f,  1.0f, 1.0f,
	 0.5f,  0.5f,  0.5f,  0.0f,  1.0f,  0.0f,  1.0f, 0.0f,
	-0.5f,  0.5f, -0.5f,  0.0f,  1.0f,  0.0f,  0.0f, 1.0f,
	-0.5f,  0.5f,  0.5f,  0.0f,  1.0f,  0.0f,  0.0f, 0.0f 
};

std::vector<glm::vec3> const cubePositions{
	// Back face
	glm::vec3(-0.5f, -0.5f, -0.5f),
	glm::vec3(0.5f,  0.5f, -0.5f),
	glm::vec3(0.5f, -0.5f, -0.5f),
	glm::vec3(0.5f,  0.5f, -0.5f),
	glm::vec3(-0.5f, -0.5f, -0.5f),
	glm::vec3(-0.5f,  0.5f, -0.5f),
	// Front face   
	glm::vec3(-0.5f, -0.5f,  0.5f),
	glm::vec3(0.5f, -0.5f,  0.5f),
	glm::vec3(0.5f,  0.5f,  0.5f),
	glm::vec3(0.5f,  0.5f,  0.5f),
	glm::vec3(-0.5f,  0.5f,  0.5f),
	glm::vec3(-0.5f, -0.5f,  0.5f),
	// Left face   
	glm::vec3(-0.5f,  0.5f,  0.5f),
	glm::vec3(-0.5f,  0.5f, -0.5f),
	glm::vec3(-0.5f, -0.5f, -0.5f),
	glm::vec3(-0.5f, -0.5f, -0.5f),
	glm::vec3(-0.5f, -0.5f,  0.5f),
	glm::vec3(-0.5f,  0.5f,  0.5f),
	//Right face  
	glm::vec3(0.5f,  0.5f,  0.5f),
	glm::vec3(0.5f, -0.5f, -0.5f),
	glm::vec3(0.5f,  0.5f, -0.5f),
	glm::vec3(0.5f, -0.5f, -0.5f),
	glm::vec3(0.5f,  0.5f,  0.5f),
	glm::vec3(0.5f, -0.5f,  0.5f),
	//Bottom face   
	glm::vec3(-0.5f, -0.5f, -0.5f),
	glm::vec3(0.5f, -0.5f, -0.5f),
	glm::vec3(0.5f, -0.5f,  0.5f),
	glm::vec3(0.5f, -0.5f,  0.5f),
	glm::vec3(-0.5f, -0.5f,  0.5f),
	glm::vec3(-0.5f, -0.5f, -0.5f),
	//Top face  
	glm::vec3(-0.5f,  0.5f, -0.5f),
	glm::vec3(0.5f,  0.5f,  0.5f),
	glm::vec3(0.5f,  0.5f, -0.5f),
	glm::vec3(0.5f,  0.5f,  0.5f),
	glm::vec3(-0.5f,  0.5f, -0.5f),
	glm::vec3(-0.5f,  0.5f,  0.5f)
};

std::vector<glm::vec3> const cubeNormals{
	glm::vec3(0.0f,  0.0f, -1.0f),
	glm::vec3(0.0f,  0.0f, -1.0f),
	glm::vec3(0.0f,  0.0f, -1.0f),
	glm::vec3(0.0f,  0.0f, -1.0f),
	glm::vec3(0.0f,  0.0f, -1.0f),
	glm::vec3(0.0f,  0.0f, -1.0f),

	glm::vec3(0.0f,  0.0f,  1.0f),
	glm::vec3(0.0f,  0.0f,  1.0f),
	glm::vec3(0.0f,  0.0f,  1.0f),
	glm::vec3(0.0f,  0.0f,  1.0f),
	glm::vec3(0.0f,  0.0f,  1.0f),
	glm::vec3(0.0f,  0.0f,  1.0f),

	glm::vec3(1.0f,  0.0f,  0.0f),
	glm::vec3(1.0f,  0.0f,  0.0f),
	glm::vec3(1.0f,  0.0f,  0.0f),
	glm::vec3(1.0f,  0.0f,  0.0f),
	glm::vec3(1.0f,  0.0f,  0.0f),
	glm::vec3(1.0f,  0.0f,  0.0f),

	glm::vec3(1.0f,  0.0f,  0.0f),
	glm::vec3(1.0f,  0.0f,  0.0f),
	glm::vec3(1.0f,  0.0f,  0.0f),
	glm::vec3(1.0f,  0.0f,  0.0f),
	glm::vec3(1.0f,  0.0f,  0.0f),
	glm::vec3(1.0f,  0.0f,  0.0f),

	glm::vec3(0.0f, -1.0f,  0.0f),
	glm::vec3(0.0f, -1.0f,  0.0f),
	glm::vec3(0.0f, -1.0f,  0.0f),
	glm::vec3(0.0f, -1.0f,  0.0f),
	glm::vec3(0.0f, -1.0f,  0.0f),
	glm::vec3(0.0f, -1.0f,  0.0f),

	glm::vec3(0.0f,  1.0f,  0.0f),
	glm::vec3(0.0f,  1.0f,  0.0f),
	glm::vec3(0.0f,  1.0f,  0.0f),
	glm::vec3(0.0f,  1.0f,  0.0f),
	glm::vec3(0.0f,  1.0f,  0.0f),
	glm::vec3(0.0f,  1.0f,  0.0f)
};

std::vector<glm::vec2> const cubeTexCoords{
	glm::vec2(0.0f, 0.0f),
	glm::vec2(1.0f, 1.0f),
	glm::vec2(1.0f, 0.0f),
	glm::vec2(1.0f, 1.0f),
	glm::vec2(0.0f, 0.0f),
	glm::vec2(0.0f, 1.0f),

	glm::vec2(0.0f, 0.0f),
	glm::vec2(1.0f, 0.0f),
	glm::vec2(1.0f, 1.0f),
	glm::vec2(1.0f, 1.0f),
	glm::vec2(0.0f, 1.0f),
	glm::vec2(0.0f, 0.0f),

	glm::vec2(1.0f, 0.0f),
	glm::vec2(1.0f, 1.0f),
	glm::vec2(0.0f, 1.0f),
	glm::vec2(0.0f, 1.0f),
	glm::vec2(0.0f, 0.0f),
	glm::vec2(1.0f, 0.0f),

	glm::vec2(1.0f, 0.0f),
	glm::vec2(0.0f, 1.0f),
	glm::vec2(1.0f, 1.0f),
	glm::vec2(0.0f, 1.0f),
	glm::vec2(1.0f, 0.0f),
	glm::vec2(0.0f, 0.0f),

	glm::vec2(0.0f, 1.0f),
	glm::vec2(1.0f, 1.0f),
	glm::vec2(1.0f, 0.0f),
	glm::vec2(1.0f, 0.0f),
	glm::vec2(0.0f, 0.0f),
	glm::vec2(0.0f, 1.0f),

	glm::vec2(0.0f, 1.0f),
	glm::vec2(1.0f, 0.0f),
	glm::vec2(1.0f, 1.0f),
	glm::vec2(1.0f, 0.0f),
	glm::vec2(0.0f, 1.0f),
	glm::vec2(0.0f, 0.0f),
};

std::vector<glm::vec3> const planePositions{
	glm::vec3(-1.0f,  1.0f,  0.0f),
	glm::vec3(-1.0f,  -1.0f,  0.0f),
	glm::vec3(1.0f, -1.0f,  0.0f),
	glm::vec3(-1.0f,  1.0f,  0.0f),
	glm::vec3(1.0f, -1.0f,  0.0f),
	glm::vec3(1.0f, 1.0f,  0.0f)
};

std::vector<glm::vec3> const planeNormals{
	glm::vec3(0.0f,  0.0f,  1.0f),
	glm::vec3(0.0f,  0.0f,  1.0f),
	glm::vec3(0.0f,  0.0f,  1.0f),
	glm::vec3(0.0f,  0.0f,  1.0f),
	glm::vec3(0.0f,  0.0f,  1.0f),
	glm::vec3(0.0f,  0.0f,  1.0f)
};

std::vector<glm::vec2> const planeTexCoords{
	glm::vec2(0.0f,  1.0f),
	glm::vec2(0.0f,  0.0f),
	glm::vec2(1.0f,  0.0f),
	glm::vec2(0.0f,  1.0f),
	glm::vec2(1.0f,  0.0f),
	glm::vec2(1.0f,  1.0),
};

GLfloat const planeVertexData[]{
	 0.5f,  0.5f,  0.0f,  0.0f,  0.5f,  0.0f,  1.0f,  0.0f,
	-0.5f,  0.5f,  0.0f,  0.0f,  0.5f,  0.0f,  0.0f,  0.0f,
	-0.5f, -0.5f,  0.0f,  0.0f,  0.5f,  0.0f,  0.0f,  1.0f,
	 0.5f,  0.5f,  0.0f,  0.0f,  0.5f,  0.0f,  1.0f,  0.0f,
	-0.5f, -0.5f,  0.0f,  0.0f,  0.5f,  0.0f,  0.0f,  1.0f,
	 0.5f, -0.5f,  0.0f,  0.0f,  0.5f,  0.0f,  1.0f,  1.0f
};

GLfloat const quadVertexData[] = {
	-1.0f,  1.0f,  0.0f, 1.0f,
	-1.0f, -1.0f,  0.0f, 0.0f,
	 1.0f, -1.0f,  1.0f, 0.0f,

	-1.0f,  1.0f,  0.0f, 1.0f,
	 1.0f, -1.0f,  1.0f, 0.0f,
	 1.0f,  1.0f,  1.0f, 1.0f
};

std::vector<glm::vec2> static scaleData(float scale, std::vector<glm::vec2> data) {
	std::vector<glm::vec2> newData;
	newData.resize(data.size());
	std::transform(data.begin(), data.end(), newData.begin(), [scale](glm::vec2 item) {return item * scale; });
	return newData;
}

std::vector<glm::vec3> static scaleData(float scale, std::vector<glm::vec3> data) {
	std::vector<glm::vec3> newData;
	newData.resize(data.size());
	std::transform(data.begin(), data.end(), newData.begin(), [scale](glm::vec3 item) {return item * scale; });
	return newData;
}
