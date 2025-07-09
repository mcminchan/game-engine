#include "ShaderManager.h"
#include "WindowConst.h"
#include "Physics.h"
#include "Model.h"
#include "Light.h"
#include "AssetManager.h"
#include "GuiWidgets.h"
#include "imgui/imgui.h"
#include "imgui/imgui_impl_glfw.h"
#include "imgui/imgui_impl_opengl3.h"
#include <iostream>
#include <PxPhysicsAPI.h>
#include <filesystem>
#include <map>
#include <glm/glm.hpp>
#include <glm/gtc/type_ptr.hpp>

using namespace physx;

namespace Gui
{
	static struct Menu
	{
		bool light;
		bool loadAsset;
	};
	static Menu menu;
	static const char* extension[3]{ "jpg", "png", "obj" };

	static void showAssets(const char* dirName, bool* opened);
	static void showMenuBar();

	extern void showWindow(bool* imguiOpen, bool* startScene)
	{
		if (menu.loadAsset) showAssets("assets", &menu.loadAsset);

		ImGuiWindowFlags windowFlag = 0;
		windowFlag |= ImGuiWindowFlags_MenuBar;
		ImGuiViewport* mainViewport = ImGui::GetMainViewport();
		ImGui::SetNextWindowPos(ImVec2(mainViewport->WorkPos.x + Window::renderWidth, mainViewport->WorkPos.y), ImGuiCond_Always);
		ImGui::SetNextWindowSize(ImVec2(Window::guiWidth, Window::guiHeight), ImGuiCond_Always);
		if (!ImGui::Begin("idiots", NULL, windowFlag))
		{
			ImGui::End();
			return;
		}
		ImFont* f = ImGui::GetFont();
		f->FontSize = 15;


		//ImGui::PushItemWidth(ImGui::GetFontSize() * -12);

		showMenuBar();
		ImGui::Spacing();
		if (*startScene == false)
		{
			ImGui::Button("start");
			if (ImGui::IsItemClicked(0))
			{
				Physics::setupScene();
				*startScene = true;
			}
		}
		else
		{
			ImGui::Button("finish");
			if (ImGui::IsItemClicked(0))
			{
				Physics::clearScene();
				*startScene = false;
			}
		}
		ImGuiIO& io = ImGui::GetIO();
		if (ImGui::CollapsingHeader("Gerneral"))
		{
			ImGui::SeparatorText("General");
			ImGui::CheckboxFlags("io.ConfigFlags: NavEnableKeyboard", &io.ConfigFlags, ImGuiConfigFlags_NavEnableKeyboard);
			ImGui::CheckboxFlags("io.ConfigFlags: NoMouse", &io.ConfigFlags, ImGuiConfigFlags_NoMouse);
			ImGui::Spacing();
		}
		if (ImGui::CollapsingHeader("SCENE"))
		{
			if (ImGui::TreeNode("Directional Light"))
			{
				directionalLightInfo();
				ImGui::TreePop();
			}

			static int selectNum = -1;
			int itemNum = 0;
			bool del = false;
			bool open = false;
			ImGuiTreeNodeFlags flag = ImGuiTreeNodeFlags_OpenOnArrow;

			ImGui::Button("delete", ImVec2(32, 32));
			if (ImGui::IsItemClicked(0) && selectNum != -1)
				del = true;
			for (auto a = AssetManager::gameObjects.begin(); a != AssetManager::gameObjects.end();)
			{

				if (itemNum == selectNum)
				{
					flag |= ImGuiTreeNodeFlags_Selected;
				}

				open = ImGui::TreeNodeEx(a->first.c_str(), flag);
				if (ImGui::IsItemClicked(0))
				{
					selectNum = itemNum;
				}
				if (ImGui::BeginDragDropTarget())
				{
					if (const ImGuiPayload* payload = ImGui::AcceptDragDropPayload("texture"))
					{

						std::string path = (char*)payload->Data;
						int slashInd = path.find_last_of("\\");
						std::string fileName = path.substr(slashInd + 1);
						path = path.substr(0, slashInd);
						a->second.setDiffuse(fileName.c_str(), path);
					}
					ImGui::EndDragDropTarget();
				}
				if (open)
				{
					gameObjectStatus(a->second);
					ImGui::TreePop();
				}
				if ((flag & ImGuiTreeNodeFlags_Selected) && del)
				{
					a = AssetManager::gameObjects.erase(a);
					selectNum = -1;
					del = false;
				}
				else
					a++;
				itemNum++;
				flag = ImGuiTreeNodeFlags_OpenOnArrow;
			}
			for (auto a = AssetManager::transparentGameObjects.begin(); a != AssetManager::transparentGameObjects.end();)
			{
				if (itemNum == selectNum)
				{
					flag |= ImGuiTreeNodeFlags_Selected;
				}

				open = ImGui::TreeNodeEx(a->first.c_str(), flag);
				if (ImGui::IsItemClicked(0))
				{
					selectNum = itemNum;
				}
				if (open)
				{
					gameObjectStatus(a->second);
					ImGui::TreePop();
				}

				if ((flag & ImGuiTreeNodeFlags_Selected) && del)
				{
					a = AssetManager::transparentGameObjects.erase(a);
					selectNum = -1;
					del = false;
				}
				else
					a++;
				itemNum++;
				flag = ImGuiTreeNodeFlags_OpenOnArrow;
			}
		}

		ImGui::End();
	}

	

	static void showMenuBar()
	{
		if (ImGui::BeginMenuBar())
		{
			if (ImGui::BeginMenu("menu"))
			{
				ImGui::MenuItem("load Asset", NULL, &menu.loadAsset);
				ImGui::EndMenu();
			}
			if (ImGui::BeginMenu("ana"))
			{
				ImGui::MenuItem("oosf", NULL, &menu.light);
				ImGui::EndMenu();
			}
			ImGui::EndMenuBar();
		}
	}

	static void showAssets(const char* dirName, bool* opened)
	{
		/*
		* 업데이트 에정 : 파일 절대경로 띄우고 버튼 누르면 원하는 경로로 한번에 이동 가능하도록 설정
		*				: 모델 파일 드래그 시 드래그중임을 표시하는 그래픽 추가(isItemActivate와 addrect를 이용하면 될듯)
		*/
		ImVec2 iconSize{ 32,32 };
		ImGui::SetNextWindowSize(ImVec2(Window::guiWidth, Window::guiHeight), ImGuiCond_Always);
		if (!ImGui::Begin("Assets", opened, ImGuiWindowFlags_MenuBar))
		{
			ImGui::End();
			return;
		}
		std::filesystem::path projectPath = std::filesystem::current_path();
		if (std::filesystem::exists(projectPath) && std::filesystem::is_directory(projectPath))
		{

			static int selectNum = -1;
			int fileNum = 0;
			bool visible = false;
			bool obj = false;
			if (projectPath.has_parent_path())
			{
				if (ImGui::Selectable(".", fileNum == selectNum, ImGuiSelectableFlags_AllowDoubleClick))
				{
					if (ImGui::IsMouseDoubleClicked(0))
					{
						std::filesystem::current_path(projectPath.parent_path());
						selectNum = -1;
					}
					else
						selectNum = fileNum;
				}
			}
			fileNum++;
			for (auto& entry : std::filesystem::directory_iterator(projectPath))
			{
				std::string fileName = entry.path().filename().string();
				int extPos = fileName.find(".");
				for (int a = 0; a < sizeof(extension) / sizeof(extension[0]); a++)
				{
					if (!fileName.compare(extPos + 1, 3, extension[a]))
					{
						visible = true;
						if (a == 2)
							obj = true;
					}
				}
				if (visible || extPos == -1)		//img or directory
				{
					if (ImGui::Selectable(fileName.c_str(), fileNum == selectNum, ImGuiSelectableFlags_AllowDoubleClick))//클릭시에만 true 리턴, 두번째 인자가 true면 선택 표시(리턴값은 클릭 안하면 false)
					{
						selectNum = fileNum;
						if (ImGui::IsMouseDoubleClicked(0) && extPos == -1)
						{
							std::filesystem::current_path(entry.path());
							selectNum = -1;
							break;
						}
					}
					if (!obj && visible)
					{
						if (ImGui::BeginDragDropSource(ImGuiDragDropFlags_None))
						{
							std::string path = entry.path().string();
							ImGui::SetDragDropPayload("texture", path.c_str(), path.size() + 1);
							ImGui::Text(fileName.c_str());
							ImGui::EndDragDropSource();
						}
					}
					if (ImGui::IsItemDeactivated() && obj)
					{
						int count = 1;
						ImVec2 mousePos = ImGui::GetMousePos();
						ImVec2 windowPos = ImGui::GetWindowPos();
						if ((mousePos.x < windowPos.x || mousePos.y < windowPos.y))
						{
							fileName = fileName.substr(0, extPos);
							std::string objectName = fileName;
							while (AssetManager::gameObjects.find(objectName) != AssetManager::gameObjects.end())
							{
								objectName = fileName + std::to_string(count++);
							}
							AssetManager::gameObjects.insert({ objectName, Model{ entry.path().string().c_str() } });
							(AssetManager::gameObjects)[objectName].setShader(&(ShaderManager::getShader(ShaderManager::basic)), ShaderManager::basic);
						}
					}
					fileNum++;
					visible = false;
					obj = false;
				}
			}
		}
		ImGui::End();
	}
}