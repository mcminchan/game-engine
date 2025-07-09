#include "AssetManager.h"
#include "Camera.h"
#include "Shader.h"
#include "Window.h"
#include "Buffers.h"
#include "Physics.h"
#include "WindowConst.h"
#include "imgui/imgui.h"
#include "imgui/imgui_impl_glfw.h"
#include "imgui/imgui_impl_opengl3.h"
#include <iostream>
#include <glad/glad.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

namespace Gui
{
	extern void showWindow(bool* imguiOpen, bool* startScene);
}
using namespace AssetManager;

void drawScene(bool isCamera)
{
	for (auto a = gameObjects.begin(); a != gameObjects.end(); a++)
	{
		if (a->first.compare("camera") || isCamera)
			a->second.draw();
	}

	for (auto a = transparentGameObjects.begin(); a != transparentGameObjects.end(); a++)
	{
		a->second.draw();
	}
}
void drawScene(const Shader& shader)
{
	for (auto a = gameObjects.begin(); a != gameObjects.end(); a++)
	{
		if(a->first.compare("camera"))
			a->second.draw(shader);
	}

	for (auto a = transparentGameObjects.begin(); a != transparentGameObjects.end(); a++)
	{
		a->second.draw(shader);
	}
}

void renderLoop()
{
	glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
	Model& camera = gameObjects["camera"];
	
	bool startScene = false;
	bool imguiOpen = false;
	Window::getCurrentTime();

	while (!Window::shouldCloseWindow())
	{

		ImGui_ImplGlfw_NewFrame();
		ImGui_ImplOpenGL3_NewFrame();
		
		ImGui::NewFrame();
		//ImGui::ShowDemoWindow();
		Gui::showWindow(&imguiOpen, &startScene);

		Window::updateTime();

		if (startScene)
			Physics::stepPhysics(Window::deltaTime);
		
		Window::input();
		
		while (Light::prepareShadowRendering())
		{
			drawScene(Light::getShadowShader());
		}
		Light::updateLightTex();

		Buffers::initEditFramebuffer();

		Camera::setCameraUniformBuffer();
		drawScene(true);

		glDepthFunc(GL_LEQUAL);
		skybox.draw();
		glDepthFunc(GL_LESS);

		Buffers::initCameraFramebuffer();
		
		Camera::setCameraUniformBuffer(glm::vec3(camera.transform.position.x, camera.transform.position.y, camera.transform.position.z)
			, Camera::getDirection(glm::vec3(camera.transform.rotation.x, camera.transform.rotation.y, camera.transform.rotation.z))
			, Camera::getUp(glm::vec3(camera.transform.rotation.x, camera.transform.rotation.y, camera.transform.rotation.z)));
		drawScene(false);

		glDepthFunc(GL_LEQUAL);
		skybox.draw();
		glDepthFunc(GL_LESS);

		glBindFramebuffer(GL_FRAMEBUFFER, 0);
		glClearColor(1.0f, 1.0f, 1.0f, 1.0f);
		glClear(GL_COLOR_BUFFER_BIT);
		glDisable(GL_DEPTH_TEST);


		frame.transform.position.y = -0.5f;
		frame.setDiffuse(Texture{ Buffers::getEditFramebufferTex(),"texture_diffuse" });		
		frame.draw();

		frame.transform.position.y = 0.5f;
		//frame.setDiffuse(Texture{ Light::getDirectionalShadowTex(),"texture_diffuse" });
		frame.setDiffuse(Texture{ Buffers::getCameraFramebufferTex(),"texture_diffuse"});
		frame.draw();


		ImGui::Render();
		ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
		Window::pollSwapWindow();
	}
}
