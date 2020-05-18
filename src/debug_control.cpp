#include "debug_control.h"

DebugControl::DebugControl(const char* glsl_version, GLFWwindow* window, Scene* scene, Renderer* renderer) : 
    scene(scene), renderer(renderer), showFrameData(false), showRenderSettings(false),
    showGBufferTextures(false), showSceneObjects(false), 
    pLightSelected(-1), dLightSelected(-1), modelSelected("")
{
    IMGUI_CHECKVERSION();
    ImGui::CreateContext();

    ImGuiIO& io = ImGui::GetIO(); (void) io;
    //this->io = ImGui::GetIO(); (void)this->io;

    ImGui::StyleColorsDark();

    ImGui_ImplGlfw_InitForOpenGL(window, true);
    ImGui_ImplOpenGL3_Init(glsl_version);

    bool show_stats;
}

DebugControl::~DebugControl() {
	ImGui_ImplOpenGL3_Shutdown();
    ImGui_ImplGlfw_Shutdown();
    ImGui::DestroyContext();
}

void DebugControl::onFrame()
{
    ImGui_ImplOpenGL3_NewFrame();
    ImGui_ImplGlfw_NewFrame();
    ImGui::NewFrame();

    this->displayBaseMenu();

    if (this->showFrameData) {
		displayFrameData();
    }

    if (this->showRenderSettings) {
        this->displayRenderSettings();
    }

    if (this->showGBufferTextures) {
        this->displayGBufferTextures();
    }

    if (this->showSceneObjects) {
        this->displaySceneObjects();
    }
}

void DebugControl::render() {
    ImGui::Render();
    ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
}

void DebugControl::displayBaseMenu()
{
    ImGui::Begin("Control");

    ImGui::Checkbox("Show frame data", &this->showFrameData);
    ImGui::Checkbox("show render settings", &this->showRenderSettings);
    ImGui::Checkbox("Show gBuffer textures", &this->showGBufferTextures);
    ImGui::Checkbox("Show scene objects", &this->showSceneObjects);

    ImGui::End();
}

void DebugControl::displayFrameData() {
    ImGuiIO& io = ImGui::GetIO();

    const float DISTANCE = 10.0f;
    ImGuiWindowFlags window_flags = ImGuiWindowFlags_NoDecoration | ImGuiWindowFlags_AlwaysAutoResize | ImGuiWindowFlags_NoSavedSettings | ImGuiWindowFlags_NoFocusOnAppearing | ImGuiWindowFlags_NoNav | ImGuiWindowFlags_NoMove;

    ImGui::SetNextWindowPos(ImVec2(10.0f, 10.0f));

    ImGui::SetNextWindowBgAlpha(0.35f);
    if (ImGui::Begin("Frame Data", &this->showFrameData, window_flags)) {
        ImGui::Text("FPS: %.1f (%.3f ms/frame)", io.Framerate, 1000.0f/io.Framerate);
        ImGui::Separator();
        if (ImGui::IsMousePosValid())
            ImGui::Text("Mouse Position: (%.1f,%.1f)", io.MousePos.x, io.MousePos.y);
        else
            ImGui::Text("Mouse Position: <invalid>");
    }
	ImGui::End();
}

void DebugControl::displayRenderSettings() {
    if (!ImGui::Begin("Render Settings", &this->showRenderSettings)) {
        ImGui::End();
        return;
    }
    ImVec2 bounds = ImVec2(this->renderer->getNearBound(), this->renderer->getFarBound());
    float fov = this->renderer->getFieldOfView();
    bool drawLights = this->renderer->getDrawLights();

    if (ImGui::InputFloat2("Near and Far Bounds", (float*) &bounds)) {
        this->renderer->setNearBound(bounds.x);
        this->renderer->setFarBound(bounds.y);
    }

    if (ImGui::SliderFloat("FoV", (float*) &fov, 45.0f, 120.0f)) {
        this->renderer->setFieldOfView(fov);
    }

    if (ImGui::Checkbox("Draw Lights", &drawLights)) {
        this->renderer->setDrawLights(drawLights);
    }

    ImGui::End();
}

void DebugControl::displayGBufferTextures()
{
    ImGui::Begin("GBuffer Textures");

    GBuffer* gBuffer = this->renderer->getGBuffer();
    ImVec2 textureSize = ImVec2(200.0f, 200.0f);
    ImVec2 uv_min = ImVec2(1.0f, 1.0f);                 // Top-left
	ImVec2 uv_max = ImVec2(0.0f, 0.0f);                 // Lower-right
	ImVec4 tint_col = ImVec4(1.0f, 1.0f, 1.0f, 1.0f);   // No tint
	ImVec4 border_col = ImVec4(1.0f, 1.0f, 1.0f, 0.5f); // 50% opaque white
    ImGui::Image((void*)gBuffer->getPositionTexture(), textureSize, uv_min, uv_max, tint_col, border_col);
    ImGui::Image((void*)gBuffer->getNormalTexture(), textureSize, uv_min, uv_max, tint_col, border_col);
    ImGui::Image((void*)gBuffer->getAlbedoSpecTexture(), textureSize, uv_min, uv_max, tint_col, border_col);

	ImGui::End();
}

void DebugControl::displaySceneObjects()
{
    ImGui::SetNextWindowSize(ImVec2(430, 450), ImGuiCond_FirstUseEver);
    if (!ImGui::Begin("Scene Objects", &this->showSceneObjects)) {
        ImGui::End();
        return;
    }

    ImGui::PushStyleVar(ImGuiStyleVar_FramePadding, ImVec2(2, 2));
    ImGui::Columns(2);
    ImGui::Separator();

	for (auto& modelIt : this->scene->getModels()) {
        displayModel(modelIt.second);
	}

    //if (ImGui::CollapsingHeader("Lights")) {
	auto pointLights = this->scene->getLightManager()->getPointLights();
	for (int i = 0; i < pointLights.size(); ++i) {
        displayPointLightControl(i);
	}

	auto directionLights = this->scene->getLightManager()->getDirectionLights();
	for (int i = 0; i < directionLights.size(); ++i) {
        displayDirectionLightControl(i);
	}

    ImGui::Columns(1);
    ImGui::Separator();
    ImGui::PopStyleVar();
    ImGui::End();
}

void DebugControl::displayModel(Model* model) {
    ImGui::PushID(model);
    ImGui::AlignTextToFramePadding();
    bool node_open = ImGui::TreeNode("Model", "Model");

    ImGui::NextColumn();
    ImGui::AlignTextToFramePadding();
    ImGui::Text(model->getName().c_str());
    ImGui::NextColumn();
    if (node_open) {
        std::string shaderName = this->renderer->getModelShader(model->getName());
        glm::vec3 mPosition = model->getPosition();
        glm::vec3 mRotation = model->getRotation();
        glm::vec3 mScale = model->getScale();
        ImVec4 position = ImVec4(mPosition.x, mPosition.y, mPosition.z, 0.0f);
        ImVec4 rotation = ImVec4(mRotation.x, mRotation.y, mRotation.z, 0.0f);
        ImVec4 scale = ImVec4(mScale.x, mScale.y, mScale.z, 0.0f);

		ImGuiTreeNodeFlags attrFlags = ImGuiTreeNodeFlags_Leaf | ImGuiTreeNodeFlags_NoTreePushOnOpen | ImGuiTreeNodeFlags_Bullet;

        for (auto mesh : model->getMeshes())
			this->displayDrawObj(mesh);

        ImGui::PushID("DiffuseColor");
        ImGui::TreeNodeEx("Shader", attrFlags);
        ImGui::NextColumn();
		if (ImGui::BeginCombo("Assigned Shader", shaderName.c_str())) {
			if (ImGui::Selectable("deferred", (shaderName == "deferred")))
				this->renderer->removeModelShader(model->getName());

            for (auto& shaderIt : this->renderer->getShaders()) {
                const bool is_selected = (shaderName == shaderIt.first);
                if (ImGui::Selectable(shaderIt.first.c_str(), is_selected))
                    this->renderer->setModelShader(model->getName(), shaderIt.first);
                    //shaderName = shaderIt.first;

                if (is_selected)
                    ImGui::SetItemDefaultFocus();
            }
            ImGui::EndCombo();
		}
        ImGui::NextColumn();
        ImGui::PopID();

        ImGui::PushID("position");
        ImGui::TreeNodeEx("Position", attrFlags);
        ImGui::NextColumn();
		if (ImGui::DragFloat3("Position", (float*)&position, 0.01f)) {
			model->setPosition(glm::vec3(position.x, position.y, position.z));
		}
        ImGui::NextColumn();
        ImGui::PopID();

        ImGui::PushID("rotation");
        ImGui::TreeNodeEx("Rotation", attrFlags);
        ImGui::NextColumn();
		if (ImGui::DragFloat3("Rotation", (float*)&rotation, 0.1f)) {
			model->setRotation(glm::vec3(rotation.x, rotation.y, rotation.z));
		}
        ImGui::NextColumn();
        ImGui::PopID();

        ImGui::PushID("scale");
        ImGui::TreeNodeEx("Scale", attrFlags);
        ImGui::NextColumn();
		if (ImGui::DragFloat3("Scale", (float*)&scale, 0.01f)) {
			model->setScale(glm::vec3(scale.x, scale.y, scale.z));
		}
        ImGui::NextColumn();
        ImGui::PopID();

        ImGui::TreePop();
    }

    ImGui::PopID();
}

void DebugControl::displayDrawObj(IDrawObj* mesh) {
    ImGui::PushID(mesh);
    ImGui::AlignTextToFramePadding();
    bool node_open = ImGui::TreeNode("Mesh");

    ImGui::NextColumn();
    ImGui::AlignTextToFramePadding();
    ImGui::Text(mesh->getName().c_str());
    ImGui::NextColumn();
    if (node_open) {
		ImGuiTreeNodeFlags attrFlags = ImGuiTreeNodeFlags_Leaf | ImGuiTreeNodeFlags_NoTreePushOnOpen | ImGuiTreeNodeFlags_Bullet;

        displayMaterial(mesh->getMaterial());

        ImGui::TreePop();
    }
    ImGui::PopID();
}

void DebugControl::displayMaterial(Material* material) {
    ImGui::PushID(material);
    ImGui::AlignTextToFramePadding();
    bool node_open = ImGui::TreeNode("Material");

    ImGui::NextColumn();
    ImGui::AlignTextToFramePadding();
    ImGui::Text(material->Name.c_str());
    ImGui::NextColumn();
    if (node_open) {
		ImGuiTreeNodeFlags attrFlags = ImGuiTreeNodeFlags_Leaf | ImGuiTreeNodeFlags_NoTreePushOnOpen | ImGuiTreeNodeFlags_Bullet;

        //ImVec4 ambientColor = ImVec4(material->AmbientColor.r, material->AmbientColor.g, material->AmbientColor.b, material->AmbientColor.t);
        //ImVec4 diffuseColor = ImVec4(material->DiffuseColor.r, material->DiffuseColor.g, material->DiffuseColor.b, material->DiffuseColor.t);
        //ImVec4 specularColor = ImVec4(material->SpecularColor.r, material->SpecularColor.g, material->SpecularColor.b, material->SpecularColor.t);

        ImGui::PushID("AmbientColor");
        ImGui::TreeNodeEx("Ambient", attrFlags);
        ImGui::NextColumn();
        ImGui::ColorEdit4("Color", (float*)&material->AmbientColor);
        ImGui::NextColumn();
        ImGui::PopID();

        ImGui::PushID("DiffuseColor");
        ImGui::TreeNodeEx("Diffuse", attrFlags);
        ImGui::NextColumn();
        ImGui::ColorEdit4("Color", (float*) &material->DiffuseColor);
        ImGui::NextColumn();
        ImGui::PopID();

        ImGui::PushID("SpecularColor");
        ImGui::TreeNodeEx("Specular", attrFlags);
        ImGui::NextColumn();
        ImGui::ColorEdit4("Color", (float*) &material->SpecularColor);
        ImGui::NextColumn();
        ImGui::PopID();

        ImGui::PushID("Shininess");
        ImGui::TreeNodeEx("Shininess", attrFlags);
        ImGui::NextColumn();
        ImGui::DragFloat("shininess", (float*) &material->Shininess);
        ImGui::NextColumn();
        ImGui::PopID();

        ImGui::PushID("Opacity");
        ImGui::TreeNodeEx("Opacity", attrFlags);
        ImGui::NextColumn();
        ImGui::DragFloat("opacity", (float*) &material->Opacity);
        ImGui::NextColumn();
        ImGui::PopID();

        ImGui::PushID("Reflectivity");
        ImGui::TreeNodeEx("Reflectivity", attrFlags);
        ImGui::NextColumn();
        ImGui::DragFloat("reflectivity", (float*) &material->Reflectivity);
        ImGui::NextColumn();
        ImGui::PopID();

        ImGui::TreePop();
    }
    ImGui::PopID();
}


void DebugControl::displayPointLightControl(int pointLightIndex) {
    PointLight* pl = this->scene->getLightManager()->getPointLights()[pointLightIndex];

    ImGui::PushID(pointLightIndex);
    ImGui::AlignTextToFramePadding();
    bool node_open = ImGui::TreeNode("Point Light", "point_light_%d", pointLightIndex);

    ImGui::NextColumn();
    ImGui::AlignTextToFramePadding();
    ImGui::Text("Point light text.");
    ImGui::NextColumn();
    if (node_open) {
		float ambientIntensity = pl->getAmbient();
		float diffuseIntensity = pl->getDiffuse();
		float specularIntensity = pl->getSpecular();
		float constant = pl->getConstant();
		float linear = pl->getLinear();
		float quadratic = pl->getQuadratic();
        glm::vec3 plc = pl->getColor();
        glm::vec3 plPos = pl->getPosition();
		ImVec4 color = ImVec4(plc.r, plc.g, plc.b, 1.0f);
		ImVec4 position = ImVec4(plPos.x, plPos.y, plPos.z, 1.0f);

		ImGuiTreeNodeFlags attrFlags = ImGuiTreeNodeFlags_Leaf | ImGuiTreeNodeFlags_NoTreePushOnOpen | ImGuiTreeNodeFlags_Bullet;

        ImGui::PushID("position");
        ImGui::TreeNodeEx("Position", attrFlags);
        ImGui::NextColumn();
		if (ImGui::DragFloat3("Position", (float*)&position, 0.01f)) {
			pl->setPosition(glm::vec3(position.x, position.y, position.z));
		}
        ImGui::NextColumn();
        ImGui::PopID();

        ImGui::PushID("color");
        ImGui::TreeNodeEx("Color", attrFlags);
        ImGui::NextColumn();
		if (ImGui::ColorEdit3("Color", (float*)&color)) {
			pl->setColor(glm::vec3(color.x, color.y, color.z));
		}
        ImGui::NextColumn();
        ImGui::PopID();

		ImGui::Text("Intensity Variables");
        ImGui::NextColumn();
        ImGui::NextColumn();

        ImGui::PushID("ambient");
        ImGui::TreeNodeEx("Ambient", attrFlags);
        ImGui::NextColumn();
		if (ImGui::DragFloat("Ambient Intensity", &ambientIntensity, 0.01f)) {
			pl->setAmbient(ambientIntensity);
		}
        ImGui::NextColumn();
        ImGui::PopID();

        ImGui::PushID("diffuse");
        ImGui::TreeNodeEx("Diffuse", attrFlags);
        ImGui::NextColumn();
		if (ImGui::DragFloat("Diffuse Intensity", &diffuseIntensity, 0.01f)) {
			pl->setDiffuse(diffuseIntensity);
		}
        ImGui::NextColumn();
        ImGui::PopID();

        ImGui::PushID("specular");
        ImGui::TreeNodeEx("Specular", attrFlags);
        ImGui::NextColumn();
		if (ImGui::DragFloat("Specular Intensity", &specularIntensity, 0.01f)) {
			pl->setSpecular(specularIntensity);
		}
        ImGui::NextColumn();
        ImGui::PopID();

		ImGui::Text("Attenuation Variables");
        ImGui::NextColumn();
        ImGui::NextColumn();

        ImGui::PushID("constant");
        ImGui::TreeNodeEx("Constant", attrFlags);
        ImGui::NextColumn();
		if (ImGui::DragFloat("Constant", &constant, 0.01f)) {
			pl->setConstant(constant);
		}
        ImGui::NextColumn();
        ImGui::PopID();

        ImGui::PushID("linear");
        ImGui::TreeNodeEx("Linear", attrFlags);
        ImGui::NextColumn();
		if (ImGui::DragFloat("Linear", &linear, 0.01f)) {
			pl->setLinear(linear);
		}
        ImGui::NextColumn();
        ImGui::PopID();

        ImGui::PushID("quadratic");
        ImGui::TreeNodeEx("Quadratic", attrFlags);
        ImGui::NextColumn();
		if (ImGui::DragFloat("Quadratic", &quadratic, 0.01f)) {
			pl->setQuadratic(quadratic);
		}
        ImGui::NextColumn();
        ImGui::PopID();

        ImGui::TreePop();
    }

    ImGui::PopID();
}

void DebugControl::displayDirectionLightControl(int directionLightIndex) {
    DirectionLight* dl = this->scene->getLightManager()->getDirectionLights()[directionLightIndex];

    ImGui::PushID(directionLightIndex);
    ImGui::AlignTextToFramePadding();
    bool node_open = ImGui::TreeNode("Direction Light", "direction_light_%d", directionLightIndex);

    ImGui::NextColumn();
    ImGui::AlignTextToFramePadding();
    ImGui::Text("Direction light text.");
    ImGui::NextColumn();
    if (node_open) {
        glm::vec3 plc = dl->getColor();
        glm::vec3 dlDir = dl->getDirection();
		ImVec4 color = ImVec4(plc.r, plc.g, plc.b, 1.0f);
		ImVec4 direction = ImVec4(dlDir.x, dlDir.y, dlDir.z, 1.0f);
		float ambientIntensity = dl->getAmbient();
		float diffuseIntensity = dl->getDiffuse();
		float specularIntensity = dl->getSpecular();

		ImGuiTreeNodeFlags attrFlags = ImGuiTreeNodeFlags_Leaf | ImGuiTreeNodeFlags_NoTreePushOnOpen | ImGuiTreeNodeFlags_Bullet;

        ImGui::PushID("direction");
        ImGui::TreeNodeEx("Direction", attrFlags);
        ImGui::NextColumn();
		if (ImGui::DragFloat3("Direction", (float*)&direction, 0.01f)) {
			dl->setDirection(glm::vec3(direction.x, direction.y, direction.z));
		}
        ImGui::NextColumn();
        ImGui::PopID();

        ImGui::PushID("color");
        ImGui::TreeNodeEx("Color", attrFlags);
        ImGui::NextColumn();
		if (ImGui::ColorEdit3("Color", (float*)&color)) {
			dl->setColor(glm::vec3(color.x, color.y, color.z));
		}
        ImGui::NextColumn();
        ImGui::PopID();

		ImGui::Text("Intensity Variables");
        ImGui::NextColumn();
        ImGui::NextColumn();

        ImGui::PushID("ambient");
        ImGui::TreeNodeEx("Ambient", attrFlags);
        ImGui::NextColumn();
		if (ImGui::DragFloat("Ambient Intensity", &ambientIntensity, 0.01f)) {
			dl->setAmbient(ambientIntensity);
		}
        ImGui::NextColumn();
        ImGui::PopID();

        ImGui::PushID("diffuse");
        ImGui::TreeNodeEx("Diffuse", attrFlags);
        ImGui::NextColumn();
		if (ImGui::DragFloat("Diffuse Intensity", &diffuseIntensity, 0.01f)) {
			dl->setDiffuse(diffuseIntensity);
		}
        ImGui::NextColumn();
        ImGui::PopID();

        ImGui::PushID("specular");
        ImGui::TreeNodeEx("Specular", attrFlags);
        ImGui::NextColumn();
		if (ImGui::DragFloat("Specular Intensity", &specularIntensity, 0.01f)) {
			dl->setSpecular(specularIntensity);
		}
        ImGui::NextColumn();
        ImGui::PopID();

        ImGui::TreePop();
    }

    ImGui::PopID();
}
