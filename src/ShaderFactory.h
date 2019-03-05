#pragma once
ShaderFactory()
{
	GLchar const *basicVPath = "src/shaders/basic.vert";
	GLchar const *basicFPath = "src/shaders/basic.frag";
	this->shaders["basic"] = Shader(basicVPath, basicFPath);

	GLchar const *textureFPath = "src/shaders/texture.frag";
	this->shaders["texture"] = Shader(basicVPath, textureFPath);

	GLchar const *depthVPath = "src/shaders/depth.vert";
	GLchar const *depthFPath = "src/shaders/depth.frag";
	this->shaders["depth"] = Shader(depthVPath, depthFPath);

	GLchar const *normalVPath = "src/shaders/normal.vert";
	GLchar const *normalFPath = "src/shaders/normal.frag";
	GLchar const *normalGPath = "src/shaders/normal.geom";
	this->shaders["normal"] = Shader(normalVPath, normalFPath, normalGPath);

	this->shaders["basic2D"] = Shader("src/shaders/basic2D.vert", "src/shaders/basic2D.frag");
	this->shaders["inverse2D"] = Shader("src/shaders/basic2D.vert", "src/shaders/inverse2D.frag");
	this->shaders["grey2D"] = Shader("src/shaders/basic2D.vert", "src/shaders/grey2D.frag");
	this->shaders["sharpen2D"] = Shader("src/shaders/basic2D.vert", "src/shaders/sharpen2D.frag");
	this->shaders["blur2D"] = Shader("src/shaders/basic2D.vert", "src/shaders/blur2D.frag");
	this->shaders["edge2D"] = Shader("src/shaders/basic2D.vert", "src/shaders/edge2D.frag");
	this->shaders["skybox"] = Shader("src/shaders/skybox.vert", "src/shaders/skybox.frag");

	GLchar const *lightVPath = "src/shaders/lights.vert";
	GLchar const *lightFPath = "src/shaders/lights.frag";
	this->shaders["light"] = Shader(lightVPath, lightFPath);

	GLchar const *highlightFPath = "src/shaders/highlight.frag";
	this->shaders["highlight"] = Shader(basicVPath, highlightFPath);

	checkGLError("shader");
}
