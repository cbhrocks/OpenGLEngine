#include "Scene.h"

Scene::Scene() :
	width(1024),
	height(1024),
	up(0.0, 1.0, 0.0),
	currentTime(0.0),
	gammaCorrection(false),
	exposure(1.0f),
	activeCamera(0)
{
	this->setup();
	this->initializeShaders();
	//this->loadObjects();
}

void Scene::loadObjects() {
	//Camera* camera = new Camera(glm::vec3(0, 0, 3), glm::vec3(0, 0, -1), this->up, 100.0f, 0.1f, this->width, this->height, 45.0f);
	//FBOManagerI* tbm = new BloomBuffer(this->getWidth(), this->getHeight(), this->shaders["bloom2D"], this->shaders["gaussianBlur2D"]);
	//FBOManagerI* tbm = new HDRBuffer(this->getWidth(), this->getHeight(), this->shaders["basic2D"]);
	//tbm->setup();
	//tbm->createVAO();
	//camera->setTBM(tbm);

	//this->cameras.push_back(camera);

	this->lightManager = new LightManager();

	BasicLight* basicLight = new BasicLight(
		glm::vec3(0.0f, 0.0f, 0.0f),
		glm::vec3(0.0f, 0.0f, 0.0f),
		glm::vec3(0.0f, 0.0f, 0.0f),
		glm::vec3(0.0f, 1.0f, 5.0f),
		&this->shaders["light"]
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
		&this->shaders["light"]
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
		&this->shaders["light"]
	);
	pointLight->addUpdateFunction("rotate", [](PointLight* pl) {
		pl->setPosition(glm::vec4(pl->getPosition(), 1.0f) * glm::rotate(glm::mat4(1.0f), glm::radians(0.25f), glm::vec3(0.0f, 1.0f, 0.0f)));
	});
	pointLight->genShadowMap();
	this->lightManager->addPointLight(*pointLight);
	SpotLight* spotLight = new SpotLight(
		glm::vec3(0.00, 0.0f, 0.0f),
		glm::vec3(0.0f, 0.0f, 0.0f),
		glm::vec3(0.0f, 0.0f, 0.0f),
		this->getActiveCamera()->getPosition(),
		glm::vec3(0.0f, 0.0f, -1.0f),
		1.0f,
		0.0014f,
		0.000007f,
		glm::cos(glm::radians(12.5f)),
		glm::cos(glm::radians(17.5f)),
		&this->shaders["light"]
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

	Model* nanosuit = new Model(std::string("objects/test/nanosuit/nanosuit.obj"), this->shaders["blinnPhongLighting"], this->gammaCorrection);
	nanosuit->setScale(glm::vec3(0.5f));
	this->models.push_back(nanosuit);

	Texture specular = createTexture(std::string("texture_specular"), glm::vec3(0.3));

	Texture shadow;
	shadow.type = std::string("texture_shadow_direction");
	shadow.id = directionLight->getDepthMap();

	Texture shadowPoint;
	shadowPoint.type = std::string("texture_shadow_cube");
	shadowPoint.id = pointLight->getDepthMap();

	Texture wallDiffuse;
	wallDiffuse.id = loadTexture(std::string("objects/test/textures/brickwall.jpg"), this->gammaCorrection);
	wallDiffuse.type = "texture_diffuse";
	wallDiffuse.path = "objects/test/textures/brickwall.png";
	Texture wallNormal;
	wallNormal.id = loadTexture(std::string("objects/test/textures/brickwall_normal.jpg"), false);
	wallNormal.type = "texture_normal";
	wallNormal.path = "objects/test/textures/brickwall_normal.png";
	std::vector<Texture> wallTextures{ wallDiffuse, wallNormal, specular, shadow, shadowPoint };

	Texture floorDiffuse;
	floorDiffuse.id = loadTexture(std::string("objects/test/textures/wood.png"), this->gammaCorrection);
	floorDiffuse.type = "texture_diffuse";
	floorDiffuse.path = "objects/test/textures/wood.png";
	std::vector<Texture> floorTextures{ floorDiffuse, specular, shadow, shadowPoint };

	Texture cubeTexture;
	cubeTexture.id = loadTexture(std::string("objects/test/textures/container.jpg"), this->gammaCorrection);
	cubeTexture.type = "texture_diffuse";
	cubeTexture.path = "objects/test/textures/container.jpg";
	std::vector<Texture> cubeTextures{ cubeTexture, specular, shadow , shadowPoint};

	Texture grassTexture;
	grassTexture.id = loadTexture(std::string("objects/test/textures/grass.png"), this->gammaCorrection);
	grassTexture.type = "texture_diffuse";
	grassTexture.path = std::string("objects/test/textures/grass.png");
	std::vector<Texture> grassTextures{ grassTexture };

	Texture windowTexture;
	windowTexture.id = loadTexture(std::string("objects/test/textures/blending_transparent_window.png"), this->gammaCorrection);
	windowTexture.type = "texture_diffuse";
	windowTexture.path = std::string("objects/test/textures/window.png");
	std::vector<Texture> windowTextures{ windowTexture };

	DrawObject* floor = DrawObject::builder()
		.setPositions(scaleData(10, planePositions))
		.setNormals(planeNormals)
		.setTexCoords(scaleData(5, planeTexCoords))
		.setTextures(wallTextures)
		.build(this->shaders["BPLightingNorm"]);
	floor->setRotation(glm::vec3(-90.0, 0.0, 0.0));
	//floor->setScale(glm::vec3(25.0));
	this->drawObjects.push_back(floor);

	std::vector<DrawObject*> walls;
	for (int i = 0; i < 3; i++) {
		DrawObject* wall = DrawObject::builder()
			.setPositions(scaleData(10, planePositions))
			.setNormals(planeNormals)
			.setTexCoords(scaleData(3, planeTexCoords))
			.setTextures(wallTextures)
			.build(this->shaders["BPLightingNorm"]);

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
		.build(this->shaders["blinnPhongLighting"]);
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
		.build(this->shaders["blinnPhongLighting"]);
	cube2->setScale(glm::vec3(2));
	cube2->setPosition(glm::vec3(2.0, 4, 3.5));
	cube2->setRotation(glm::vec3(45.0f, 45.0f, 0.0f));
	this->drawObjects.push_back(cube2);

	DrawObject* grass = DrawObject::builder()
		.setPositions(planePositions)
		.setNormals(planeNormals)
		.setTexCoords(planeTexCoords)
		.setTextures(grassTextures)
		.build(this->shaders["trans"]);
	grass->setPosition(glm::vec3(0.0, 1.0, 1.0));
	this->drawObjects.push_back(grass);

	DrawObject* window1 = DrawObject::builder()
		.setPositions(planePositions)
		.setNormals(planeNormals)
		.setTexCoords(planeTexCoords)
		.setTextures(windowTextures)
		.setTransparent(true)
		.build(this->shaders["trans"]);
	window1->setPosition(glm::vec3(0.5, 1.0, 2.0));
	this->drawObjects.push_back(window1);

	DrawObject* window2 = DrawObject::builder()
		.setPositions(planePositions)
		.setNormals(planeNormals)
		.setTexCoords(planeTexCoords)
		.setTextures(windowTextures)
		.setTransparent(true)
		.build(this->shaders["trans"]);
	window2->setPosition(glm::vec3(0.5, 1.0, 3.0));
	this->drawObjects.push_back(window2);
}

//FBOManager* Scene::getFBOManager() const {
//	return this->fboManager;
//}
//
//void Scene::setFBOManager(FBOManager* fbo) {
//	this->fboManager = fbo;
//}

LightManager* Scene::getLightManager() const {
	return this->lightManager;
}

void Scene::setLightManager(LightManager* lightManager) {
	this->lightManager = lightManager;
}

void Scene::clearObjects() {
	this->lightManager = new LightManager();
	this->models.clear();
	this->drawObjects.clear();
	this->cameras.clear();
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
	this->getActiveCamera()->getTBM()->setActive();
	this->drawModels();
	this->drawHighlight();
	//this->drawModels(this->shaders["tbn"]);
	this->getActiveCamera()->getTBM()->Draw();
}

void Scene::drawShadows() {
	this->lightManager->drawShadowMaps([this](const Shader& shader) {
		this->drawModels(shader);
	});

	glBindFramebuffer(GL_FRAMEBUFFER, 0);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	glViewport(0, 0, this->width, this->height);
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
		//add transparent models to list sorted by distance
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
	glStencilMask(0x00);
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
			//predraw functions
			if (this->drawObjects.at(i)->canHighlight()) {
				glStencilFunc(GL_ALWAYS, 1, 0xFF);
				glStencilMask(0xFF);
			}
			this->drawObjects.at(i)->Draw();
			//postdraw functions
			if (this->drawObjects.at(i)->canHighlight()) {
				glStencilFunc(GL_NOTEQUAL, 1, 0xFF);
				glStencilMask(0x00);
			}
		}
		//add transparent models to list sorted by distance
		else {
			float distance = glm::length(this->getActiveCamera()->getPosition() - this->drawObjects.at(i)->getPosition());
			sorted[distance] = this->drawObjects.at(i);
		}
	}
	//draw transparent models
	for (std::map<float, DrawObject*>::reverse_iterator it = sorted.rbegin(); it != sorted.rend(); ++it)
	{
		it->second->getShader().Use();
		it->second->UploadUniforms();
		//predraw functions
		if (it->second->canHighlight()) {
			glStencilFunc(GL_ALWAYS, 1, 0xFF);
			glStencilMask(0xFF);
		}
		it->second->Draw();
		//postdraw functions
		if (it->second->canHighlight()) {
			glStencilFunc(GL_NOTEQUAL, 1, 0xFF);
			glStencilMask(0x00);
		}
	}
}

void Scene::drawHighlight()
{
	//this->camera->uploadUniforms(shaders["highlight"]);
	glDisable(GL_DEPTH_TEST);
	for (int i = 0; i < this->drawObjects.size(); i++) {
		if (this->drawObjects.at(i)->canHighlight()) {
			glm::vec3 currentScale = this->drawObjects.at(i)->getScale();
			this->drawObjects.at(i)->setScale(currentScale * 1.1f);
			this->drawObjects.at(i)->UploadUniforms(shaders["highlight"]);
			this->drawObjects.at(i)->Draw(shaders["highlight"]);
			this->drawObjects.at(i)->setScale(currentScale);
		}
	}
	glEnable(GL_DEPTH_TEST);
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

size_t Scene::getWidth() const
{
	return this->width;
}

void Scene::setWidth(size_t width)
{
	this->width = width;
}

size_t Scene::getHeight() const
{
	return this->height;
}

void Scene::setHeight(size_t height)
{
	this->height = height;
}

void Scene::setGammaCorrection(bool gamma)
{
	this->gammaCorrection = gamma;
	this->clearObjects();
	this->loadObjects();
}

bool Scene::getGammaCorrection() const
{
	return this->gammaCorrection;
}

float Scene::getExposure() const
{
	return this->exposure;
}

void Scene::scaleModels(const glm::vec3& scale)
{
	for (int i = 0; i < this->models.size(); i++)
		this->models.at(i)->setScale(scale);
}

void Scene::initializeShaders()
{
	this->shaders["basic"] = Shader("src/shaders/basic.vert", "src/shaders/basic.frag").setUniformBlock("Camera", 1);
	checkGLError("Scene::initializeShaders -- basic");

	this->shaders["texture"] = Shader("src/shaders/basic.vert", "src/shaders/texture.frag").setUniformBlock("Camera", 1);
	checkGLError("Scene::initializeShaders -- texture");

	this->shaders["trans"] = Shader("src/shaders/basic.vert", "src/shaders/trans.frag").setUniformBlock("Camera", 1);
	checkGLError("Scene::initializeShaders -- trans");

	this->shaders["depth"] = Shader("src/shaders/depth.vert", "src/shaders/depth.frag").setUniformBlock("Camera", 1);
	checkGLError("Scene::initializeShaders -- depth");

	this->shaders["normal"] = Shader("src/shaders/normal.vert", "src/shaders/normal.frag", "src/shaders/normal.geom").setUniformBlock("Camera", 1);
	checkGLError("Scene::initializeShaders -- normal");

	this->shaders["tbn"] = Shader("src/shaders/tbn.vert", "src/shaders/tbn.frag", "src/shaders/tbn.geom").setUniformBlock("Camera", 1);
	checkGLError("Scene::initializeShaders -- tbn");

	this->shaders["faceNormal"] = Shader("src/shaders/faceNormal.vert", "src/shaders/faceNormal.frag", "src/shaders/faceNormal.geom").setUniformBlock("Camera", 1);
	checkGLError("Scene::initializeShaders -- faceNormal");

	this->shaders["skybox"] = Shader("src/shaders/skybox.vert", "src/shaders/skybox.frag");
	checkGLError("Scene::initializeShaders -- skybox");

	this->shaders["light"] = Shader("src/shaders/basic.vert", "src/shaders/light.frag").setUniformBlock("Camera", 1);
	checkGLError("Scene::initializeShaders -- light");

	this->shaders["shadowDepth"] = Shader("src/shaders/shadowDepth.vert", "src/shaders/shadowDepth.frag");

	this->shaders["shadowDebug"] = Shader("src/shaders/shadowDebug.vert", "src/shaders/shadowDebug.frag").setUniformBlock("Camera", 1);
	checkGLError("Scene::initializeShaders -- shadowDebug");

	this->shaders["phongLighting"] = Shader("src/shaders/lighting.vert", "src/shaders/phongLighting.frag")
		.setUniformBlock("Camera", 1)
		.setUniformBlock("Lights", 2);
	checkGLError("Scene::initializeShaders -- phongLighting");

	this->shaders["blinnPhongLighting"] = Shader("src/shaders/lighting.vert", "src/shaders/blinnPhongLighting.frag")
		.setUniformBlock("Camera", 1)
		.setUniformBlock("Lights", 2);
	checkGLError("Scene::initializeShaders -- blinnPhongLighting");

	this->shaders["BPLightingNorm"] = Shader("src/shaders/BPLightingNorm.vert", "src/shaders/BPLightingNorm.frag")
		.setUniformBlock("Camera", 1)
		.setUniformBlock("Lights", 2);
	checkGLError("Scene::initializeShaders -- blinnPhongLighting");

	this->shaders["bloom2D"] = Shader("src/shaders/bloom2D.vert", "src/shaders/bloom2D.frag")
		.setUniformBlock("Scene", 0);
	checkGLError("Scene::initializeShaders -- bloom2D");

	this->shaders["highlight"] = Shader("src/shaders/basic.vert", "src/shaders/highlight.frag")
		.setUniformBlock("Camera", 1);
	checkGLError("Scene::initializeShaders -- highlight");

	this->shaders["reflection"] = Shader("src/shaders/reflection.vert", "src/shaders/reflection.frag")
		.setUniformBlock("Camera", 1);
	checkGLError("Scene::initializeShaders -- reflection");

	this->shaders["explode"] = Shader("src/shaders/explode.vert", "src/shaders/texture.frag", "src/shaders/explode.geom")
		.setUniformBlock("Scene", 0)
		.setUniformBlock("Camera", 1);
	checkGLError("Scene::initializeShaders -- explode");

	this->shaders["basic2D"] = Shader("src/shaders/basic2D.vert", "src/shaders/basic2D.frag")
		.setUniformBlock("Scene", 0);
	checkGLError("Scene::initializeShaders -- basic2D");

	this->shaders["gaussianBlur2D"] = Shader("src/shaders/gaussianBlur2D.vert", "src/shaders/gaussianBlur2D.frag")
		.setUniformBlock("Scene", 0);
	checkGLError("Scene::initializeShaders -- gaussianBlur2D");

	this->shaders["hdr2D"] = Shader("src/shaders/basic2D.vert", "src/shaders/hdr2D.frag")
		.setUniformBlock("Scene", 0);
	checkGLError("Scene::initializeShaders -- hdr2D");

	this->shaders["inverse2D"] = Shader("src/shaders/basic2D.vert", "src/shaders/inverse2D.frag")
		.setUniformBlock("Scene", 0);
	checkGLError("Scene::initializeShaders -- inverse2D");

	this->shaders["grey2D"] = Shader("src/shaders/basic2D.vert", "src/shaders/grey2D.frag")
		.setUniformBlock("Scene", 0);
	checkGLError("Scene::initializeShaders -- grey2D");

	this->shaders["sharpen2D"] = Shader("src/shaders/basic2D.vert", "src/shaders/sharpen2D.frag")
		.setUniformBlock("Scene", 0);
	checkGLError("Scene::initializeShaders -- sharpen2D");

	this->shaders["blur2D"] = Shader("src/shaders/basic2D.vert", "src/shaders/blur2D.frag")
		.setUniformBlock("Scene", 0);
	checkGLError("Scene::initializeShaders -- blur2D");

	this->shaders["edge2D"] = Shader("src/shaders/basic2D.vert", "src/shaders/edge2D.frag")
		.setUniformBlock("Scene", 0);
	checkGLError("Scene::initializeShaders -- edge2D");
}

GLuint ubo;
Skybox* skybox;
LightManager* lightManager;
std::vector<Model*> models;
std::vector<DrawObject*> drawObjects;
std::vector<Camera*> cameras;
int activeCamera;
bool running;
Shader lightShader;

void Scene::setup() {
	this->setupUbo();
}

void Scene::setupUbo() {
	int uboSize = 4// time
		+ 4 //gamma
		+ 4; // exposure
	glGenBuffers(1, &this->ubo);
	glBindBuffer(GL_UNIFORM_BUFFER, this->ubo);
	glBufferData(GL_UNIFORM_BUFFER, uboSize, NULL, GL_STATIC_DRAW);
	glBindBuffer(GL_UNIFORM_BUFFER, 0);
	glBindBufferRange(GL_UNIFORM_BUFFER, 0, this->ubo, 0, uboSize);
	checkGLError("Scene::setupUbo");
}

void Scene::updateUbo() {
	float time = this->currentTime;
	int gamma = this->gammaCorrection;
	float exposure = this->exposure;
	glBindBuffer(GL_UNIFORM_BUFFER, this->ubo);
	glBufferSubData(GL_UNIFORM_BUFFER, 0, 4, &time);
	glBufferSubData(GL_UNIFORM_BUFFER, 4, 4, &gamma);
	glBufferSubData(GL_UNIFORM_BUFFER, 8, 4, &exposure);
	glBindBuffer(GL_UNIFORM_BUFFER, 0);
	checkGLError("Scene::updateUbo");
}
