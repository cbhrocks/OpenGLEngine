#include "model.h"

/*  Functions   */
// constructor, expects a filepath to a 3D model.
Model::Model(
	std::string const path
) : position(glm::vec3(0)), scale(glm::vec3(1)), rotation(glm::vec3(0)), isTransparent(false)
{
	Assimp::Importer importer;
	const aiScene* scene = importer.ReadFile(path, aiProcess_Triangulate | aiProcess_FlipUVs | aiProcess_CalcTangentSpace);
	// check for errors
	if (!scene || scene->mFlags & AI_SCENE_FLAGS_INCOMPLETE || !scene->mRootNode) // if is Not Zero
	{
		if (!scene) fprintf(stderr, "%s\n", importer.GetErrorString());
	}
	else {
		processNode(path.substr(0, path.find_last_of("/")), scene->mRootNode, scene);
	}
}

Model::Model(
	std::vector<VertexData> vertices, 
	std::vector<GLuint> indices, 
	std::map<GLuint, std::string> textures
) : position(glm::vec3(0)), scale(glm::vec3(1)), rotation(glm::vec3(0)), isTransparent(false)
{
	this->meshes.push_back(std::unique_ptr<Mesh>(new Mesh(vertices, indices, textures)));
}

// draws the model, and thus all its meshes
void Model::Draw()
{
	this->Draw(*this->shader);
}

void Model::Draw(const Shader& shader)
{
	shader.Use();
	for (unsigned int i = 0; i < meshes.size(); i++)
		meshes[i]->Draw(shader);
}

void Model::uploadUniforms()
{
	this->uploadUniforms(*this->shader);
}

void Model::uploadUniforms(const Shader& shader)
{
	checkGLError("Model::uploadUniforms -- start");
	shader.Use();
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
	checkGLError("Model::uploadUniforms -- end");
}

const bool Model::getTransparent() const {
	return this->isTransparent;
}

Model* Model::setTransparent(bool isTransparent) {
	this->isTransparent = isTransparent;
	return this;
}

const glm::vec3 Model::getPosition() const {
	return this->position;
}

Model* Model::setPosition(glm::vec3 position)
{
	this->position = position;
	return this;
}

const glm::vec3 Model::getScale() const {
	return this->scale;
}

Model* Model::setScale(glm::vec3 scale)
{
	this->scale = scale;
	return this;
}

const glm::vec3 Model::getRotation() const {
	return this->rotation;
}

Model* Model::setRotation(glm::vec3 rotation) {
	this->rotation = rotation;
	return this;
}

const Shader* Model::getShader()
{
	return this->shader;
}

Model* Model::setShader(const Shader* shader)
{
	this->shader = shader;
	return this;
}

/*  Functions   */

// processes a node in a recursive fashion. Processes each individual mesh located at the node and repeats this process on its children nodes (if any).
void Model::processNode(std::string const &path, aiNode *node, const aiScene *scene)
{
	// process each mesh located at the current node
	for (unsigned int i = 0; i < node->mNumMeshes; i++)
	{
		// the node object only contains indices to index the actual objects in the scene. 
		// the scene contains all the data, node is just to keep stuff organized (like relations between nodes).
		aiMesh* mesh = scene->mMeshes[node->mMeshes[i]];
		processMesh(path, mesh, scene);
	}
	// after we've processed all of the meshes (if any) we then recursively process each of the children nodes
	for (unsigned int i = 0; i < node->mNumChildren; i++)
	{
		processNode(path, node->mChildren[i], scene);
	}
}

void Model::processMesh(std::string const &path, aiMesh *mesh, const aiScene *scene)
{
	// data to fill
	std::vector<VertexData> vertices;
	std::vector<GLuint> indices;

	// Walk through each of the mesh's vertices
	for (unsigned int i = 0; i < mesh->mNumVertices; i++)
	{
		VertexData vertex;
		//since assimp uses its own vector class that doesn't directly convert to glm's vec3 class so we transfer the data glm::vec3 first.
		// positions
		vertex.Position = glm::vec3(
			mesh->mVertices[i].x,
			mesh->mVertices[i].y,
			mesh->mVertices[i].z
		);
		// normals
		vertex.Normal = glm::vec3(
			mesh->mNormals[i].x,
			mesh->mNormals[i].y,
			mesh->mNormals[i].z
		);
		if (mesh->mTextureCoords[0]) // does the mesh contain texture coordinates?
		{
			// texture coordinates
			// a vertex can contain up to 8 different texture coordinates. We thus make the assumption that we won't 
			// use models where a vertex can have multiple texture coordinates so we always take the first set (0).
			vertex.uv = glm::vec2(mesh->mTextureCoords[0][i].x, mesh->mTextureCoords[0][i].y);
		}
		else
			vertex.uv = glm::vec2(0.0f, 0.0f);

		// tangent and bitangent
		vertex.Tangent = glm::vec3(
			mesh->mTangents[i].x,
			mesh->mTangents[i].y,
			mesh->mTangents[i].z
		);
		vertex.Bitangent = glm::vec3(
			mesh->mBitangents[i].x,
			mesh->mBitangents[i].y,
			mesh->mBitangents[i].z
		);
		vertices.push_back(vertex);
	}

	// now walk through each of the mesh's faces (a face is a mesh its triangle) and retrieve the corresponding vertex indices.
	for (GLuint i = 0; i < mesh->mNumFaces; i++)
	{
		// retrieve all indices of the face and store them in the indices vector
		for (unsigned int j = 0; j < mesh->mFaces[i].mNumIndices; j++) {
			indices.push_back(mesh->mFaces[i].mIndices[j]);
		}
	}

	// process materials
	aiMaterial* material = scene->mMaterials[mesh->mMaterialIndex];

	// load in the coeficients.
	float shininess;
	aiColor4D specularColor;
	if (AI_SUCCESS != aiGetMaterialFloat(material, AI_MATKEY_SHININESS, &shininess)) {
		shininess = 0.0f;
	}
	if (AI_SUCCESS != aiGetMaterialColor(material, AI_MATKEY_COLOR_SPECULAR, &specularColor)) {
		specularColor.r = 0;
		specularColor.g = 0;
		specularColor.b = 0;
		specularColor.a = 0;
	}
	aiMaterialProperty** mProperties = material->mProperties;

	// we assume a convention for sampler names in the shaders. Each diffuse texture should be named
	// as 'texture_diffuseN' where N is a sequential number ranging from 1 to MAX_SAMPLER_NUMBER. 
	// Same applies to other texture as the following list summarizes:
	// diffuse: texture_diffuseN
	// specular: texture_specularN
	// normal: texture_normalN
	std::map<GLuint, std::string> textures;

	// 1. diffuse maps
	std::map<GLuint, std::string> diffuseTextures = loadMaterialTextures(path, material, aiTextureType_DIFFUSE);
	textures.insert(diffuseTextures.begin(), diffuseTextures.end());
	// 2. specular maps
	std::map<GLuint, std::string> specularTextures = loadMaterialTextures(path, material, aiTextureType_SPECULAR);
	textures.insert(specularTextures.begin(), specularTextures.end());
	// 3. normal maps
	std::map<GLuint, std::string> normalTextures = loadMaterialTextures(path, material, aiTextureType_HEIGHT);
	textures.insert(normalTextures.begin(), normalTextures.end());
	// 4. height maps
	std::map<GLuint, std::string> heightMaps = loadMaterialTextures(path, material, aiTextureType_AMBIENT);
	textures.insert(heightMaps.begin(), heightMaps.end());

	// return a mesh object created from the extracted mesh data
	meshes.push_back(std::unique_ptr<Mesh>(new Mesh(vertices, indices, textures)));
}

// checks all material textures of a given type and loads the textures if they're not loaded yet.
// the required info is returned as a Texture struct.
std::map<GLuint, std::string> Model::loadMaterialTextures(const std::string& path, aiMaterial *material, aiTextureType type)
{
	//std::vector<Texture> textures;
	std::map<GLuint, std::string> textures;
	for (unsigned int i = 0; i < material->GetTextureCount(type); i++)
	{
		GLenum format; // format of the texture on creation
		GLuint textureId; // actual texture id
		std::string mode; // type of texture thats being imported

		aiString mat_path; // local path of material texture from obj
		material->GetTexture(type, i, &mat_path);

		auto it = textures_loaded.find(mat_path.C_Str());
		if (it != textures_loaded.end())
		{
			textures.insert(std::make_pair(it->second.id, it->second.type));
		}
		else
		{   // if texture hasn't been loaded already, load it

			int width, height, channel_num; // texture width and height, number of channels in the image (RGBa)
			std::string filename = path + "/" + mat_path.C_Str(); // actual path to file in local file system
			unsigned char *image_data = stbi_load(filename.c_str(), &width, &height, &channel_num, 0); // image data of texture
			if (!image_data) {
				fprintf(stderr, "%s %s\n", "Failed to load texture", filename.c_str());
			}

			switch (channel_num)
			{
				case 1: format = GL_ALPHA;     break;
				//case 2 : format = GL_LUMINANCE; break;
				case 3: format = GL_RGB;       break;
				case 4: format = GL_RGBA;      break;
			}

			glGenTextures(1, &textureId);
			glBindTexture(GL_TEXTURE_2D, textureId);

			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_MIRRORED_REPEAT);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

			glTexImage2D(GL_TEXTURE_2D, 0, format, width, height, 0, format, GL_UNSIGNED_BYTE, image_data);
			glGenerateMipmap(GL_TEXTURE_2D);

			stbi_image_free(image_data);

			switch (type) {
			case aiTextureType_DIFFUSE: mode = "diffuse"; break;
			case aiTextureType_SPECULAR: mode = "specular"; break;
			case aiTextureType_HEIGHT: mode = "normal"; break;
			case aiTextureType_AMBIENT: mode = "height"; break;
			}

			Texture texture;
			texture.id = textureId;
			texture.type = mode;
			textures.insert(std::make_pair(textureId, mode));
			this->textures_loaded.insert(std::make_pair(mat_path.C_Str(), texture));
		}
	}
	return textures;
}
