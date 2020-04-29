#pragma once

#include <glad/glad.h> 

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/matrix_inverse.hpp>
#include <assimp/Importer.hpp>
#include <assimp/scene.h>
#include <assimp/postprocess.h>

#include <string>
#include <fstream>
#include <sstream>
#include <iostream>
#include <map>
#include <vector>

#include "stb_image.h"
#include "mesh.h"
#include "shader.h"
#include "TextureManager.h"

class Model 
{
    public:
        /*  Functions   */
        // constructor, expects a filepath to a 3D model.
        // loads a model with supported ASSIMP extensions from file and stores the resulting meshes in the meshes vector.
		Model(
			std::string const path
		);
		//constructor expects vertex data, indices, and textures
		Model(
			std::vector<VertexData> vertices,
			std::vector<GLuint> indices,
			std::map<GLuint, std::string> textures
		);

        // drastd::ws the model, and thus all its meshes
		void Draw();

		void Draw(const Shader& shader);

		void uploadUniforms();

		void uploadUniforms(const Shader& shader);

		const Shader* getShader();
		Model* setShader(const Shader* shader);

		const bool getTransparent() const;
		Model* setTransparent(bool isTransparent);

		const glm::vec3 getPosition() const;
		Model* setPosition(glm::vec3 position);

		const glm::vec3 getScale() const;
		Model* setScale(glm::vec3 scale);

		const glm::vec3 getRotation() const;
		Model* setRotation(glm::vec3 rotation);

    private:
        std::vector<std::unique_ptr<Mesh>> meshes;
        std::map<std::string, Texture> textures_loaded;	// stores all the textures loaded so far, optimization to make sure textures aren't loaded more than once.
        std::string directory; //the directory that the model is loaded from.
		glm::vec3 position, scale, rotation; //the world location attributes of the model.
		const Shader* shader; //the shader used to render the model in forward rendering.
		bool isTransparent; //whether or not the model has transparent textures.

        /*  Functions   */
        // processes a std::node in a recursive fashion. Processes each individual mesh located at the node and repeats this process on its children nodes (if any).
		void processNode(std::string const &path, aiNode *node, const aiScene *scene);

		void processMesh(std::string const &path, aiMesh *mesh, const aiScene *scene);

        // checks all material textures of a given type and loads the textures if they're not loaded yet.
        // the required info is returned as a Texture struct.
		std::map<GLuint, std::string> loadMaterialTextures(const std::string& path, aiMaterial *mat, aiTextureType type);
};
