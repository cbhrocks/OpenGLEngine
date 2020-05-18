#include "scene.h"
#include "renderer.h"

void setupBasic(Scene* scene, Renderer* renderer) {
	Camera* camera = new Camera(
		glm::vec3(0, 0, 0),
		glm::vec3(0, 1, 0),
		0.0f,
		-90.0f
	);
	scene->addCamera(camera);

	LightManager* lm = new LightManager();
	DirectionLight* directionLight = new DirectionLight(
		glm::vec3(1.0f, 1.0f, 1.0f),
		glm::vec3(0.25f, -1.0f, -0.25f),
		0.2f,
		0.5f,
		1.0f
	);
	lm->addDirectionLight(directionLight);
	ShadowMap* shadowMap = new ShadowMap(
		glm::vec3(0.0f, 20.0f, 0.0f),
		directionLight->getDirectionRef(),
		1080,
		1080,
		20,
		20,
		0.1f,
		25.0f
	);
	lm->addShadowMap(shadowMap);
	PointLight* pointLight = new PointLight(
		glm::vec3(0.0f, 8.0f, 5.0f),
		glm::vec3(1, 1, 1),
		0.3f,
		1.5f,
		2.5f,
		1.0f,
		0.35f,
		0.44f
	);
	lm->addShadowCubeMap(new ShadowCubeMap(
		pointLight->getPosition(),
		//glm::vec3(0.0f, 8.0f, 5.0f),
		1080,
		1080,
		0.1f,
		15.0f
	));
	scene->addUpdateFunction("plight-rotate", [](Scene* scene) {
		PointLight* pl = scene->getLightManager()->getPointLights()[0];
		pl->setPosition(glm::vec4(pl->getPosition(), 1.0f) * glm::rotate(glm::mat4(1.0f), glm::radians(0.25f), glm::vec3(0.0f, 1.0f, 0.0f)));
	});
	scene->addUpdateFunction("plight-shadow-follow", [](Scene* scene) {
		PointLight* pl = scene->getLightManager()->getPointLights()[0];
		ShadowCubeMap* shadow = scene->getLightManager()->getShadowCubeMaps()[0];
		shadow->setPosition(pl->getPosition());
	});
	lm->addPointLight(pointLight);
	SpotLight* spotLight = new SpotLight(
		glm::vec3(0.0f),
		glm::vec3(0.0f, 0.0f, -1.0f),
		glm::vec3(1.0, 1.0, 1.0),
		0.2f,
		1.5f,
		3.0f,
		1.0f,
		0.0014f,
		0.000007f,
		glm::cos(glm::radians(12.5f)),
		glm::cos(glm::radians(17.5f))
	);
	scene->addUpdateFunction("slight-follow-camera", [](Scene* scene) {
		SpotLight* sl = scene->getLightManager()->getSpotLights()[0];
		Camera* camera = scene->getActiveCamera();
		sl->setPosition(camera->position);
		sl->setDirection(camera->front);
	});
	lm->addSpotLight(spotLight);

	lm->createUniformBlock();

	scene->setLightManager(lm);

	scene->setModel((new Model("nanosuit", std::string("objects/test/nanosuit/nanosuit.obj"), aiProcess_FlipUVs)));
	//render.setModelShader("nanosuit", "directionalShadows");

	//scene->setModel(("box1", new Model(std::string("objects/test/wood_box/wood_box.obj")))
	//	->setPosition(glm::vec3(-4.0, 1.5, 3.5))
	//	->setScale(glm::vec3(2.0f))
	//);

	//scene->setModel(("box2", new Model(std::string("objects/test/wood_box/wood_box.obj")))
	//	->setPosition(glm::vec3(2.0, 4, 3.5))
	//);

	//scene->setModel(("grass", new Model( std::string("objects/test/grass_square/grass_square.obj")))
	//	->setPosition(glm::vec3( 0, 0.5, 0 ))
	//	->setRotation(glm::vec3( 90, 0, 0 ))
	//	->setTransparent(true)
	//);

	//scene->setModel(("window1", new Model( std::string("objects/test/window/window.obj")))
	//	->setPosition(glm::vec3(0, 1.0, -3.0))
	//	->setRotation(glm::vec3(90, 0, 0))
	//	->setTransparent(true)
	//);

	//scene->setModel(("window2", new Model( std::string("objects/test/window/window.obj")))
	//	->setPosition(glm::vec3(0, 1.0, -2.0))
	//	->setRotation(glm::vec3(90, 0, 0))
	//	->setTransparent(true)
	//);

	scene->setModel(("floor", new Model("floor", std::string("objects/test/wood_floor/wood_floor.obj")))
		->setScale(glm::vec3(10))
	);
	renderer->setModelShader("floor", "directionalShadows");

	scene->setModel(("wall", new Model("wall", std::string("objects/test/brick_wall/brick_wall.obj")))
		->setPosition(glm::vec3(0, 10, -10))
		->setScale(glm::vec3(10))
		->setRotation(glm::vec3(90, 0, 0))
	);
	//render.setModelShader("wall", "BPLightingNorm");

	scene->setModel(("backpack", new Model("backpack", std::string("objects/test/Backpack/backpack.obj")))
		->setPosition(glm::vec3(0.0f, 2.0f, 3.0f))
	);
	renderer->setModelShader("backpack", "directionalShadows");

	//std::vector<std::unique_ptr<IDrawObj>> sphereMeshes;
	//sphereMeshes.push_back(std::make_unique<Sphere>(1, 36, 18, false));
	//Model* sphere = new Model(sphereMeshes);
	//sphere->setPosition(glm::vec3(3.0f, 2.0f, 2.0f));
	//scene->setModel(sphere);
	//render.setModelShader("Sphere", "material");

	Model* sphere = new Model("sphere", std::make_unique<Icosphere>("sphere", 1.0f, 2, true));
	sphere->setPosition(glm::vec3(3.0f, 2.0f, 2.0f));
	scene->setModel(sphere);
	renderer->setModelShader("sphere", "material");
}