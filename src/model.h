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
        /*  Model Data */
        std::vector<Texture> textures_loaded;	// stores all the textures loaded so far, optimization to make sure textures aren't loaded more than once.
        std::vector<Mesh> meshes;
        std::string directory;
		glm::vec3& position, scale;
		const Shader* shader;
		bool gammaCorrection;

        /*  Functions   */
        // constructor, expects a filepath to a 3D model.
		Model(
			std::string const &path,
			const Shader* shader,
			const bool& gammaCorrection,
			glm::vec3& position = glm::vec3(0.0f),
			glm::vec3& scale = glm::vec3(1.0f)
		);

        // drastd::ws the model, and thus all its meshes
		void Draw();

		void Draw(const Shader& shader);

		void setPosition(glm::vec3 position);

		void setScale(glm::vec3 scale);

		void uploadUniforms();

		void uploadUniforms(const Shader& shader);

		void setShader(const Shader* shader);

		const Shader* getShader();

    private:
        /*  Functions   */
        // loads a model with supported ASSIMP extensions from file and stores the resulting meshes in the meshes vector.
		void loadModel(std::string const &path);

        // processes a std::node in a recursive fashion. Processes each individual mesh located at the node and repeats this process on its children nodes (if any).
		void processNode(aiNode *node, const aiScene *scene);

		Mesh processMesh(aiMesh *mesh, const aiScene *scene);

        // checks all material textures of a given type and loads the textures if they're not loaded yet.
        // the required info is returned as a Texture struct.
		std::vector<Texture> loadMaterialTextures(aiMaterial *mat, aiTextureType type, std::string typeName);
};
