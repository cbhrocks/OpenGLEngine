#include "imgui_impl_glfw.h"
#include "imgui_impl_opengl3.h"

#include "scene.h"
#include "renderer.h"

class DebugControl {
public:
	DebugControl(const char* glsl_version, GLFWwindow* window, Scene* scene, Renderer* renderer);
	~DebugControl();

	void onFrame();

	void render();

	//ImGuiIO& getIO() { return this->io; }

private:
	void displayBaseMenu();

	void displayFrameData();

	void displayRenderSettings();

	void displayGBufferTextures();

	void displaySceneObjects();

	void displayModel(Model* model);

	void displayDrawObj(IDrawObj* mesh);

	void displayMaterial(Material* material);

	void displayPointLightControl(int pointLightIndex);

	void displayDirectionLightControl(int pointLightIndex);

	Scene* scene;
	Renderer* renderer;

	//ImGuiIO& io;

	bool showFrameData, showGBufferTextures, showSceneObjects, showRenderSettings;
	int pLightSelected, dLightSelected;
	std::string modelSelected;
};

