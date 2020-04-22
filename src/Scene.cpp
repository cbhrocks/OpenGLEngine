#include "Scene.h"

Scene::Scene() :
	up(0.0, 1.0, 0.0),
	currentTime(0.0),
	gammaCorrection(2.2f),
	exposure(1.0f),
	activeCamera(0),
	bloom(true)
{
	this->setup();
	this->initializeShaders();
	//this->loadObjects();
}

void Scene::loadObjects() {
	this->lightManager = new LightManager();

	BasicLight* basicLight = new BasicLight(
		glm::vec3(0.0f, 0.0f, 0.0f),
		glm::vec3(0.0f, 0.0f, 0.0f),
		glm::vec3(0.0f, 0.0f, 0.0f),
		glm::vec3(0.0f, 1.0f, 5.0f),
		&this->shaders["light"],
		&this->shaders["shadowCubeDepth"]
	);
	this->lightManager->addBasicLight(*basicLight);
	DirectionLight* directionLight = new DirectionLight(
		//glm::vec3(0.2f, 0.2f, 0.2f),
		//glm::vec3(0.5f, 0.5f, 0.5f),
		//glm::vec3(1.0f, 1.0f, 1.0f),
		glm::vec3(0.0f, 0.0f, 0.0f),
		glm::vec3(0.0f, 0.0f, 0.0f),
		glm::vec3(0.0f, 0.0f, 0.0f),
		glm::vec3(-5.0f, 10.0f, 5.0f),
		glm::vec3(0.25f, -0.5f, -0.25f),
		&this->shaders["light"],
		&this->shaders["shadowDepth"]
	);
	this->lightManager->addDirectionLight(*directionLight);
	PointLight* pointLight = new PointLight(
		glm::vec3(0.2f, 0.2f, 0.2f),
		glm::vec3(1.5f, 1.5f, 1.5f),
		glm::vec3(3.0f, 3.0f, 3.0f),
		//glm::vec3(0.2f, 0.2f, 0.2f),
		//glm::vec3(200.0f, 200.0f, 200.0f),
		//glm::vec3(3.0f, 3.0f, 3.0f),
		glm::vec3(0.0f, 8.0f, 5.0f),
		1.0f,
		0.09f,
		0.032f,
		&this->shaders["light"],
		&this->shaders["shadowCubeDepth"]
	);
	pointLight->addUpdateFunction("rotate", [](PointLight* pl) {
		pl->setPosition(glm::vec4(pl->getPosition(), 1.0f) * glm::rotate(glm::mat4(1.0f), glm::radians(0.25f), glm::vec3(0.0f, 1.0f, 0.0f)));
	});
	this->lightManager->addPointLight(*pointLight);
	SpotLight* spotLight = new SpotLight(
		glm::vec3(0.2f, 0.2f, 0.2f),
		glm::vec3(1.5f, 1.5f, 1.5f),
		glm::vec3(3.0f, 3.0f, 3.0f),
		this->getActiveCamera()->getPosition(),
		glm::vec3(0.0f, 0.0f, -1.0f),
		1.0f,
		0.0014f,
		0.000007f,
		glm::cos(glm::radians(12.5f)),
		glm::cos(glm::radians(17.5f)),
		&this->shaders["light"],
		nullptr
	);
	spotLight->addUpdateFunction("followCamera", [camera = this->getActiveCamera()](SpotLight* sl) {
		sl->setPosition(camera->position);
		sl->setDirection(camera->look);
	});
	this->lightManager->addSpotLight(*spotLight);
	this->lightManager->createUniformBlock();

	std::vector<std::string> faces{
		"objects/test/textures/skybox/right.jpg",
		"objects/test/textures/skybox/left.jpg",
		"objects/test/textures/skybox/top.jpg",
		"objects/test/textures/skybox/bottom.jpg",
		"objects/test/textures/skybox/front.jpg",
		"objects/test/textures/skybox/back.jpg"
	};
	Texture skyboxTexture;
	skyboxTexture.id = loadSkyboxTexture(faces);
	skyboxTexture.type = "texture_skybox";
	this->skybox = new Skybox(skyboxTexture, this->shaders["skybox"]);

	Model* nanosuit = new Model(std::string("objects/test/nanosuit/nanosuit.obj"), &this->shaders["blinnPhongLighting"], true, glm::vec3(0.0f), glm::vec3(0.5f));
	this->models.push_back(nanosuit);

	Texture specular = createTexture(std::string("texture_specular"), glm::vec3(0.3));

	Texture shadowDirection;
	shadowDirection.type = std::string("texture_shadow_direction");
	shadowDirection.id = directionLight->getDepthMap();

	Texture shadowPoint;
	shadowPoint.type = std::string("texture_shadow_cube");
	shadowPoint.id = pointLight->getDepthMap();

	Texture wallDiffuse;
	wallDiffuse.id = loadTexture(std::string("objects/test/textures/brickwall.jpg"), true);
	wallDiffuse.type = "texture_diffuse";
	wallDiffuse.path = "objects/test/textures/brickwall.png";
	Texture wallNormal;
	wallNormal.id = loadTexture(std::string("objects/test/textures/brickwall_normal.jpg"), true);
	wallNormal.type = "texture_normal";
	wallNormal.path = "objects/test/textures/brickwall_normal.png";
	std::vector<Texture> wallTextures{ wallDiffuse, wallNormal, specular, shadowDirection, shadowPoint };

	Texture floorDiffuse;
	floorDiffuse.id = loadTexture(std::string("objects/test/textures/wood.png"), true);
	floorDiffuse.type = "texture_diffuse";
	floorDiffuse.path = "objects/test/textures/wood.png";
	std::vector<Texture> floorTextures{ floorDiffuse, specular, shadowDirection, shadowPoint };

	Texture cubeTexture;
	cubeTexture.id = loadTexture(std::string("objects/test/textures/container.jpg"), true);
	cubeTexture.type = "texture_diffuse";
	cubeTexture.path = "objects/test/textures/container.jpg";
	std::vector<Texture> cubeTextures{ cubeTexture, specular, shadowDirection , shadowPoint};

	Texture grassTexture;
	grassTexture.id = loadTexture(std::string("objects/test/textures/grass.png"), true);
	grassTexture.type = "texture_diffuse";
	grassTexture.path = std::string("objects/test/textures/grass.png");
	std::vector<Texture> grassTextures{ grassTexture };

	Texture windowTexture;
	windowTexture.id = loadTexture(std::string("objects/test/textures/blending_transparent_window.png"), true);
	windowTexture.type = "texture_diffuse";
	windowTexture.path = std::string("objects/test/textures/window.png");
	std::vector<Texture> windowTextures{ windowTexture };

	DrawObject* floor = DrawObject::builder()
		.setPositions(scaleData(10, planePositions))
		.setNormals(planeNormals)
		.setTexCoords(scaleData(5, planeTexCoords))
		.setTextures(floorTextures)
		.build(&this->shaders["blinnPhongLighting"]);
	floor->setRotation(glm::vec3(-90.0, 0.0, 0.0));
	//floor->setScale(glm::vec3(25.0));
	this->drawObjects.push_back(floor);

	for (int i = 0; i < 3; i++) {
		DrawObject* wall = DrawObject::builder()
			.setPositions(scaleData(10, planePositions))
			.setNormals(planeNormals)
			.setTexCoords(scaleData(3, planeTexCoords))
			.setTextures(wallTextures)
			.build(&this->shaders["BPLightingNorm"]);

		i == 0 ? wall->setRotation(glm::vec3(0.0, 90.0, 0.0)) :
			i == 1 ? wall->setRotation(glm::vec3(0.0, 0.0, 0.0)) :
			wall->setRotation(glm::vec3(0.0, -90.0, 0.0));

		i == 0 ? wall->setPosition(glm::vec3(-10.0, 10.0, 0.0)) :
			i == 1 ? wall->setPosition(glm::vec3(0.0, 10.0, -10.0)) :
			wall->setPosition(glm::vec3(10.0, 10.0, 0.0));

		this->drawObjects.push_back(wall);
	}

	DrawObject* cube = DrawObject::builder()
		.setPositions(cubePositions)
		.setNormals(cubeNormals)
		.setTexCoords(cubeTexCoords)
		.setTextures(cubeTextures)
		.setHighlight(true)
		//.setReflect(0.5f)
		//.setSkybox(&this->skybox->texture)
		.build(&this->shaders["blinnPhongLighting"]);
	cube->setScale(glm::vec3(3));
	cube->setPosition(glm::vec3(-4.0, 1.5, 3.5));
	this->drawObjects.push_back(cube);

	DrawObject* cube2 = DrawObject::builder()
		.setPositions(cubePositions)
		.setNormals(cubeNormals)
		.setTexCoords(cubeTexCoords)
		.setTextures(cubeTextures)
		//.setHighlight(true)
		//.setReflect(0.5f)
		//.setSkybox(&this->skybox->texture)
		.build(&this->shaders["blinnPhongLighting"]);
	cube2->setScale(glm::vec3(2));
	cube2->setPosition(glm::vec3(2.0, 4, 3.5));
	cube2->setRotation(glm::vec3(45.0f, 45.0f, 0.0f));
	this->drawObjects.push_back(cube2);

	DrawObject* grass = DrawObject::builder()
		.setPositions(planePositions)
		.setNormals(planeNormals)
		.setTexCoords(planeTexCoords)
		.setTextures(grassTextures)
		.build(&this->shaders["trans"]);
	grass->setPosition(glm::vec3(0.0, 1.0, 1.0));
	this->drawObjects.push_back(grass);

	DrawObject* window1 = DrawObject::builder()
		.setPositions(planePositions)
		.setNormals(planeNormals)
		.setTexCoords(planeTexCoords)
		.setTextures(windowTextures)
		.setTransparent(true)
		.build(&this->shaders["trans"]);
	window1->setPosition(glm::vec3(0.5, 1.0, 2.0));
	this->drawObjects.push_back(window1);

	DrawObject* window2 = DrawObject::builder()
		.setPositions(planePositions)
		.setNormals(planeNormals)
		.setTexCoords(planeTexCoords)
		.setTextures(windowTextures)
		.setTransparent(true)
		.build(&this->shaders["trans"]);
	window2->setPosition(glm::vec3(0.5, 1.0, 3.0));
	this->drawObjects.push_back(window2);
}

LightManager* Scene::getLightManager() const {
	return this->lightManager;
}

void Scene::setLightManager(LightManager* lightManager) {
	this->lightManager = lightManager;
}

void Scene::addModel(Model* model) {
	this->models.push_back(model);
}

void Scene::clearObjects() {
	this->lightManager = new LightManager();
	this->models.clear();
	this->drawObjects.clear();
	//this->cameras.clear();
}

void Scene::onFrame()
{
	this->lightManager->runUpdateFuncs();
	this->lightManager->updateUniformBlock();
	this->getActiveCamera()->updateUniformBlock();
}

void Scene::uploadSkyboxUniforms(const Shader& shader) {
	shader.Use();
	glm::mat4 projection = this->getActiveCamera()->getProjectionMatrix();
	glm::mat4 view = glm::mat4(glm::mat3(this->getActiveCamera()->getViewMatrix()));

	shader.setMat4("P", projection);
	shader.setMat4("V", view);

	checkGLError("upload skybox uniforms");
}

void Scene::draw() {
	this->drawShadows();
	Camera* camera = this->getActiveCamera();
	if (camera->getTBM() != nullptr) {
		this->getActiveCamera()->getTBM()->setActive();
	}
	this->drawModels();
	//this->drawModels(this->shaders["tbn"]);
	if (camera->getTBM() != nullptr) {
		this->getActiveCamera()->getTBM()->Draw();
	}
}

void Scene::drawShadows() {
	this->lightManager->drawShadowMaps([this](const Shader& shader) {
		this->drawModels(shader);
	});

	glBindFramebuffer(GL_FRAMEBUFFER, 0);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	//glViewport(0, 0, this->width, this->height);
}

void Scene::drawSkybox() {
	uploadSkyboxUniforms(skybox->getShader());
	skybox->uploadUniforms();
	skybox->Draw();
}

void Scene::drawModels(const Shader& shader) {
	for (int i = 0; i < this->models.size(); i++) {
		this->models.at(i)->uploadUniforms(shader);
		this->models.at(i)->Draw(shader);
	}
	//this->lightManager->drawLights();

	std::map<float, DrawObject*> sorted;
	//draw non transparent models
	for (int i = 0; i < this->drawObjects.size(); i++) {
		if (!this->drawObjects.at(i)->isTransparent()) {
			this->drawObjects.at(i)->UploadUniforms(shader);
			this->drawObjects.at(i)->Draw(shader);
		}
		//add transparent models to map with key distance since maps are sorted by key by default.
		else {
			float distance = glm::length(this->getActiveCamera()->getPosition() - this->drawObjects.at(i)->getPosition());
			sorted[distance] = this->drawObjects.at(i);
		}
	}
	//draw transparent models
	for (std::map<float, DrawObject*>::reverse_iterator it = sorted.rbegin(); it != sorted.rend(); ++it)
	{
		it->second->UploadUniforms(shader);
		it->second->Draw(shader);
	}
}

void Scene::drawModels()
{
	for (int i = 0; i < this->models.size(); i++) {
		this->models.at(i)->uploadUniforms();
		this->models.at(i)->Draw();
	}

	this->lightManager->drawLights();

	std::map<float, DrawObject*> sorted;
	//draw non transparent models
	for (int i = 0; i < this->drawObjects.size(); i++) {
		if (!this->drawObjects.at(i)->isTransparent()) {
			this->drawObjects.at(i)->UploadUniforms();
			this->drawObjects.at(i)->Draw();
		}
		//add transparent models to map with key distance since maps are sorted by key by default.
		else {
			float distance = glm::length(this->getActiveCamera()->getPosition() - this->drawObjects.at(i)->getPosition());
			sorted[distance] = this->drawObjects.at(i);
		}
	}
	//draw transparent models
	//TODO: look into putting the highlight drawing elsewhere, or make it independent of the drawobj's shader
	for (std::map<float, DrawObject*>::reverse_iterator it = sorted.rbegin(); it != sorted.rend(); ++it)
	{
		//it->second->getShader()->Use();
		it->second->UploadUniforms();
		it->second->Draw();
	}
}

void Scene::timeStep(double t)
{
	this->currentTime = t;
	this->updateUbo();
}

Camera* Scene::getActiveCamera() const
{
	return this->cameras.at(this->activeCamera);
}

void Scene::setActiveCamera(const int &activeCamera)
{
	this->activeCamera = activeCamera;
}

void Scene::addCamera(Camera* camera)
{
	this->cameras.push_back(camera);
}

glm::vec3 Scene::getUp() const
{
	return this->up;
}

void Scene::setUp(glm::vec3 up)
{
	this->up = up;
}

void Scene::setGammaCorrection(float gamma)
{
	this->gammaCorrection = gamma;
	printf("Gamma correction set to: %f", gamma);
	this->clearObjects();
	this->loadObjects();
}

float Scene::getGammaCorrection() const
{
	return this->gammaCorrection;
}

void Scene::setExposure(float exposure)
{
	 this->exposure = exposure;
}

float Scene::getExposure() const
{
	return this->exposure;
}

void Scene::setBloom(bool bloom)
{
	this->bloom = bloom;
}

bool Scene::getBloom() const
{
	return this->bloom;
}

void Scene::scaleModels(const glm::vec3& scale)
{
	for (int i = 0; i < this->models.size(); i++)
		this->models.at(i)->setScale(scale);
}

void Scene::initializeShaders()
{
	this->shaders.insert(std::pair<std::string, const Shader>("basic", Shader("src/shaders/basic.vert", "src/shaders/basic.frag").setUniformBlock("Camera", 1)));
	this->shaders.insert(std::pair<std::string, const Shader>("texture", Shader("src/shaders/basic.vert", "src/shaders/texture.frag").setUniformBlock("Camera", 1)));
	this->shaders.insert(std::pair<std::string, const Shader>("trans", Shader("src/shaders/basic.vert", "src/shaders/trans.frag").setUniformBlock("Camera", 1)));
	this->shaders.insert(std::pair<std::string, const Shader>("depth", Shader("src/shaders/depth.vert", "src/shaders/depth.frag").setUniformBlock("Camera", 1)));
	this->shaders.insert(std::pair<std::string, const Shader>("normal", Shader("src/shaders/normal.vert", "src/shaders/normal.frag", "src/shaders/normal.geom").setUniformBlock("Camera", 1)));
	this->shaders.insert(std::pair<std::string, const Shader>("tbn", Shader("src/shaders/tbn.vert", "src/shaders/tbn.frag", "src/shaders/tbn.geom").setUniformBlock("Camera", 1)));
	this->shaders.insert(std::pair<std::string, const Shader>("faceNormal", Shader("src/shaders/faceNormal.vert", "src/shaders/faceNormal.frag", "src/shaders/faceNormal.geom").setUniformBlock("Camera", 1)));
	this->shaders.insert(std::pair<std::string, const Shader>("skybox", Shader("src/shaders/skybox.vert", "src/shaders/skybox.frag")));
	this->shaders.insert(std::pair<std::string, const Shader>("light", Shader("src/shaders/basic.vert", "src/shaders/light.frag").setUniformBlock("Camera", 1)));
	this->shaders.insert(std::pair<std::string, const Shader>("shadowDepth", Shader("src/shaders/shadowDepth.vert", "src/shaders/shadowDepth.frag")));
	this->shaders.insert(std::pair<std::string, const Shader>("shadowCubeDepth", Shader("src/shaders/shadowDepthCube.vert", "src/shaders/shadowDepthCube.frag", "src/shaders/shadowDepthCube.geom")));
	this->shaders.insert(std::pair<std::string, const Shader>("shadowDebug", Shader("src/shaders/shadowDebug.vert", "src/shaders/shadowDebug.frag").setUniformBlock("Camera", 1)));
	this->shaders.insert(std::pair<std::string, const Shader>("phongLighting", Shader("src/shaders/lighting.vert", "src/shaders/phongLighting.frag").setUniformBlock("Camera", 1).setUniformBlock("Lights", 2)));
	this->shaders.insert(std::pair<std::string, const Shader>("blinnPhongLighting", Shader("src/shaders/lighting.vert", "src/shaders/blinnPhongLighting.frag").setUniformBlock("Scene", 0).setUniformBlock("Camera", 1).setUniformBlock("Lights", 2)));
	this->shaders.insert(std::pair<std::string, const Shader>("BPLightingNorm", Shader("src/shaders/BPLightingNorm.vert", "src/shaders/BPLightingNorm.frag").setUniformBlock("Scene", 0).setUniformBlock("Camera", 1).setUniformBlock("Lights", 2)));
	this->shaders.insert(std::pair<std::string, const Shader>("highlight", Shader("src/shaders/basic.vert", "src/shaders/highlight.frag").setUniformBlock("Camera", 1)));
	this->shaders.insert(std::pair<std::string, const Shader>("reflection", Shader("src/shaders/reflection.vert", "src/shaders/reflection.frag").setUniformBlock("Camera", 1)));
	this->shaders.insert(std::pair<std::string, const Shader>("explode", Shader("src/shaders/explode.vert", "src/shaders/texture.frag", "src/shaders/explode.geom").setUniformBlock("Scene", 0).setUniformBlock("Camera", 1)));
	checkGLError("Scene::initializeShaders -- Scene shaders");
}

void Scene::setup() {
	this->setupUbo();
}

void Scene::setupUbo() {
	int uboSize = 4// time
		+ 4 //gamma
		+ 4 //exposure
		+ 4; //bloom
	glGenBuffers(1, &this->ubo);
	glBindBuffer(GL_UNIFORM_BUFFER, this->ubo);
	glBufferData(GL_UNIFORM_BUFFER, uboSize, NULL, GL_STATIC_DRAW);
	glBindBuffer(GL_UNIFORM_BUFFER, 0);
	glBindBufferRange(GL_UNIFORM_BUFFER, 0, this->ubo, 0, uboSize);
	checkGLError("Scene::setupUbo");
}

void Scene::updateUbo() {
	float time = this->currentTime;
	float gamma = this->gammaCorrection;
	float exposure = this->exposure;
	bool bloom = this->bloom;
	glBindBuffer(GL_UNIFORM_BUFFER, this->ubo);
	glBufferSubData(GL_UNIFORM_BUFFER, 0, 4, &time);
	glBufferSubData(GL_UNIFORM_BUFFER, 4, 4, &gamma);
	glBufferSubData(GL_UNIFORM_BUFFER, 8, 4, &exposure);
	glBufferSubData(GL_UNIFORM_BUFFER, 12, 4, &bloom);
	glBindBuffer(GL_UNIFORM_BUFFER, 0);
	checkGLError("Scene::updateUbo");
}
