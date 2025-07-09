#include "Light.h"
#include "WindowConst.h"
#include "imgui/imgui.h"
#include "GuiWidgets.h"
#include "ShaderManager.h"
#include "glm/glm.hpp"
#include "glm/gtc/matrix_transform.hpp"
#include "glm/gtc/type_ptr.hpp"

namespace Gui
{
	template <typename T>
	static void dragVec3(std::string label, T& vec, float speed, float min, float max)
	{
		float widgetWidthRatio = 0.3f;
		ImGui::SetNextItemWidth(Window::guiWidth * widgetWidthRatio);
		ImGui::DragFloat((label + ".x").c_str(), &vec.x, speed, min, max, "x: %f");
		ImGui::SameLine();
		ImGui::SetNextItemWidth(Window::guiWidth * widgetWidthRatio);
		ImGui::DragFloat((label + ".y").c_str(), &vec.y, speed, min, max, "y: %f");
		ImGui::SameLine();
		ImGui::SetNextItemWidth(Window::guiWidth * widgetWidthRatio);
		ImGui::DragFloat((label + ".z").c_str(), &vec.z, speed, min, max, "z: %f");
	}
	template <typename T>
	static void dragPointLight(T& light)
	{
		ImGui::Text("position");
		dragVec3("##SpotLight.position", light.position, 0.1f, 0.0f, 0.0f);

		ImGui::Text("diffuse");
		dragVec3("##SpotLight.diffuse", light.diffuse, 0.01f, 0.0f, 1.0f);

		ImGui::Text("specular");
		dragVec3("##SpotLight.specular", light.specular, 0.01f, 0.0f, 1.0f);

		ImGui::Text("embient");
		dragVec3("##SpotLight.embient", light.ambient, 0.01f, 0.0f, 1.0f);

		ImGui::Text("constant");
		ImGui::SameLine();
		ImGui::DragFloat("##SpotLight.constant", &light.constant, 0.01f, 0, 0, "%.3f");

		ImGui::Text("linear");
		ImGui::SameLine();
		ImGui::DragFloat("##SpotLight.linear", &light.linear, 0.01f, 0, 0, "%.3f");

		ImGui::Text("quadratic");
		ImGui::SameLine();
		ImGui::DragFloat("##SpotLight.quadratic", &light.quadratic, 0.01f, 0, 0, "%.3f");
	}

	void directionalLightInfo()
	{
		float widgetWidthRatio = 0.3f;
		Light::DirectionalLight directionalL = Light::getDirectionalLight();

		ImGui::Text("diffuse");
		dragVec3("##directionalL.diffuse", directionalL.diffuse, 0.01f, 0.0f, 1.0f);

		ImGui::Text("specular");
		dragVec3("##directionalL.specular", directionalL.specular, 0.01f, 0.0f, 1.0f);

		ImGui::Text("ambient");
		dragVec3("##directionalL.ambient", directionalL.ambient, 0.01f, 0.0f, 1.0f);

		static glm::vec3 rotation{ 0,0,0 };
		ImGui::Text("direction");
		dragVec3("##directionalL.direction", rotation, 0.1f, 0.0f, 0.0f);

		glm::quat q{ glm::radians(rotation) };
		directionalL.direction=q*glm::vec3{ 1,0,0 };
		directionalL.up = q * glm::vec3{ 0,1,0 };
		ImGui::Text("x : %f y : %f z : %f", directionalL.direction.x, directionalL.direction.y, directionalL.direction.z);
		Light::updateLight(directionalL);

	}

	void gameObjectStatus(Model& gameObject)
	{

		ImGui::SeparatorText("Transform");
		float widgetWidthRatio = 0.3f;
		PxVec3 position;
		PxVec3 rotation;
		if (gameObject.mRigidBody.isRigidBody())
		{
			gameObject.setTransform(gameObject.mRigidBody.getTransform());
		}

		position = gameObject.transform.position;
		rotation = gameObject.transform.rotation;

		ImGui::Text("position");
		dragVec3("##gameObject.position", position, 0.01f, 0.0f, 0.0f);
		gameObject.setPosition(position);


		ImGui::Text("rotation");
		dragVec3("##gameObject.rotation", rotation, 0.1f, 0.0f, 0.0f);
		gameObject.setRotation(rotation);

		ImGui::Text("scale");
		PxVec3& scale = gameObject.transform.scale;
		dragVec3("##gameObject.scale", scale, 0.001f, 0.0f, 0.0f);

		ImGui::SeparatorText("Texture");
		const char* name = gameObject.getTextureName();
		if (name)
			ImGui::Text("current texture: %s", gameObject.getTextureName());
		else
			ImGui::Text("current texture: NULL");

		ImGui::SeparatorText("Shader");
		ImGui::Checkbox("view normal", &gameObject.env.viewNormal);
		int currentShader = gameObject.getShaderNum();

		ImGui::Combo("Shader", &currentShader, ShaderManager::nameList, ShaderManager::size);
		gameObject.setShader(&ShaderManager::getShader((ShaderManager::Name)currentShader), currentShader);


		static bool componentList = false;
		if (gameObject.mRigidBody.isCollider())
		{
			bool openColliderStat = true;
			//static bool open = ImGui::CollapsingHeader("Box Collider", &openColliderStat);
			//static initialize는 프로그램 실행 시 한번만 실행된다. 그래서 위의 명령으로는 open이 불가능해서 헤더가 나오지 않는다.
			if (ImGui::CollapsingHeader("Box Collider", &openColliderStat))
			{
				PxGeometryHolder& geometry = gameObject.mRigidBody.getCollider();
				if (geometry.getType() == PxGeometryType::eBOX)
				{
					PxVec3& size = geometry.box().halfExtents;

					ImGui::Text("size");
					dragVec3("##boxCollider.size", size, 0.01f, 0.0f, 0.0f);
				}
				ImGui::Checkbox("render Collider", &gameObject.env.isRenderCollider);
			}
			
			if (openColliderStat == false)
			{
				gameObject.removeCollider();
			}
			
		}
		if (gameObject.mRigidBody.isDynamic())
		{
			bool openRigidDynamicStat = true;
			if (ImGui::CollapsingHeader("Rigid Dynamic", &openRigidDynamicStat));
			if (!openRigidDynamicStat)
				gameObject.mRigidBody.setStatic();
		}
		if (gameObject.mLightType != Light::none)
		{
			bool lightExist = true;
			if (ImGui::CollapsingHeader("Light", &lightExist))
			{
				if (gameObject.mLightType == Light::spotLight)
				{

					Light::SpotLight light = Light::getSpotLight(gameObject.mLightIndex);
					light.innerCutoff = glm::degrees(acos(light.innerCutoff));
					light.outerCutoff = glm::degrees(acos(light.outerCutoff));

					ImGui::Text("direction");
					dragVec3("##SpotLight.direction", light.direction, 0.1f, 0.0f, 0.0f);

					ImGui::Text("offset");
					dragVec3("##SpotLight.offset", light.offset, 1.0f, 0.0f, 0.0f);

					ImGui::Text("innerCutoff Angle");
					ImGui::SameLine();
					ImGui::DragFloat("##spotLight.innerCutoff", &light.innerCutoff, 0.1f, 0, 360, "%f");

					ImGui::Text("outerCutoff Angle");
					ImGui::SameLine();
					ImGui::DragFloat("##spotLight.outerCutoff", &light.outerCutoff, 0.1f, 0, 360, "%f");

					light.innerCutoff = cos(glm::radians(light.innerCutoff));
					light.outerCutoff = cos(glm::radians(light.outerCutoff));
					dragPointLight(light);

					gameObject.setLight(light);
				}
				else
				{
					Light::PointLight light = Light::getPointLight(gameObject.mLightIndex);

					dragPointLight(light);
					gameObject.setLight(light);
				}
			}
			if (!lightExist)
			{
				gameObject.removeLight();
			}
			
		}

		if (!componentList)
		{
			ImGui::Button("add component");
			if (ImGui::IsItemClicked(0))
			{
				componentList = true;
			}
		}
		else
		{
			const char* list[4] = { "RigidDynamic","Box Collider", "PointLight", "SpotLight" };
			int item = -1;
			ImGui::ListBox("##ComponentList", &item, list, sizeof(list) / sizeof(list[0]));
			if (item >= 0)
				componentList = false;
			switch (item)
			{
			case 0:
				gameObject.setRigidDynamic();
				break;
			case 1:
				gameObject.setCollider(PxBoxGeometry(gameObject.transform.scale));
				break;
			case 2:
				gameObject.addLight(Light::PointLight{ });
				break;
			case 3:
				gameObject.addLight(Light::SpotLight{});
			}
		}
	}
}