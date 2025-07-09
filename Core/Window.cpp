#include "Camera.h"
#include "Buffers.h"
#include "imgui/imgui.h"
#include "imgui/imgui_impl_glfw.h"
#include "imgui/imgui_impl_opengl3.h"
#include <glm/gtc/type_ptr.hpp>
#include <iostream>
#include <glad/glad.h>
#include <GLFW/glfw3.h>

namespace Window
{
	enum direction { LEFT, FORWARD, RIGHT, BACK, UP, DOWN };

	extern double deltaTime=0;

	static GLFWwindow* window;
	
	static double lastTime;
	static double currentTime;

	extern int guiWidth = 600;
	extern int guiHeight = 500;
	extern int windowWidth = 1920;
	extern int windowHeight = 1080;
	extern int renderWidth = 1320;
	extern int renderHeight = 1080;
	
	extern float renderWidthRatio = 0.6875f;
	extern float renderHeightRatio = 1;
	extern float guiHeightRatio = 0.4629f;


	void updateTime()
	{
		lastTime = currentTime;
		currentTime = glfwGetTime();
		deltaTime = currentTime-lastTime;
	}
	double getCurrentTime()
	{
		currentTime = glfwGetTime();
		return currentTime;
	}
	void frame_resize_callback(GLFWwindow* window, int width, int height)
	{
		windowWidth = width;
		windowHeight = height;

		renderWidth = windowWidth * renderWidthRatio;
		renderHeight = height;

		guiWidth = windowWidth - renderWidth;
		guiHeight = windowHeight * guiHeightRatio;

		Buffers::updateRenderFrame();

		glViewport(0, 0, renderWidth, renderHeight);
	}

	void cameraRotate(GLFWwindow* window, double xPos, double yPos)
	{
		ImGuiIO& io = ImGui::GetIO();
		io.AddMousePosEvent(xPos, yPos);

		if (io.WantCaptureMouse == false)
			Camera::cameraRotate(xPos, yPos);
	}
	void zoom(GLFWwindow* window, double xScroll, double yScroll)
	{
		ImGuiIO& io = ImGui::GetIO();
		io.AddMouseWheelEvent(xScroll, yScroll);
		if (io.WantCaptureMouse == false)
			Camera::zoom(xScroll, yScroll);
	}

	void mouseClickCallback(GLFWwindow* window, int button, int action, int mode)
	{
		ImGuiViewport* viewport = ImGui::GetMainViewport();
		ImGuiIO& io = ImGui::GetIO();
		io.AddMouseButtonEvent(button, action);
		if (io.WantCaptureMouse == false)
		{
			if (button == GLFW_MOUSE_BUTTON_RIGHT)
			{
				if (action == GLFW_PRESS)
				{
					glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
					Camera::movable = true;
				}
				else
				{
					glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_NORMAL);
					Camera::movable = false;
				}
			}
		}

	}
	void input()
	{
		if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
			glfwSetWindowShouldClose(window, true);
		if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS)
		{
			Camera::cameraMove(direction::FORWARD, deltaTime);
		}
		if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS)
		{
			Camera::cameraMove(direction::BACK, deltaTime);
		}
		if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS)
		{
			Camera::cameraMove(direction::LEFT, deltaTime);
		}
		if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS)
		{
			Camera::cameraMove(direction::RIGHT, deltaTime);
		}
		if (glfwGetKey(window, GLFW_KEY_Q) == GLFW_PRESS)
		{
			Camera::cameraMove(direction::DOWN, deltaTime);
		}
		if (glfwGetKey(window, GLFW_KEY_E) == GLFW_PRESS)
		{
			Camera::cameraMove(direction::UP, deltaTime);
		}
	}

	void initWindow()
	{
		glfwInit();
		glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
		glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
		glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

		window = glfwCreateWindow(windowWidth, windowHeight, "sibal", NULL, NULL);
		if (window == NULL)
		{
			std::cout << "failed" << std::endl;
			glfwTerminate();
			return;
		}
		glfwMakeContextCurrent(window);
		if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
		{
			std::cout << "GLAD failed" << std::endl;
			return;
		}

		glViewport(0, 0, renderWidth, renderHeight);

		glfwSetFramebufferSizeCallback(window, frame_resize_callback);

		glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_NORMAL);
		glfwSetMouseButtonCallback(window, mouseClickCallback);
		glfwSetCursorPosCallback(window, cameraRotate);
		glfwSetScrollCallback(window, zoom);

		IMGUI_CHECKVERSION();
		ImGui::CreateContext();
		ImGuiIO& io = ImGui::GetIO();
		io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;
		ImGui_ImplGlfw_InitForOpenGL(window, true);
		ImGui_ImplOpenGL3_Init();

	}

	void terminateWindow()
	{
		glfwTerminate();
		ImGui_ImplOpenGL3_Shutdown();
		ImGui_ImplGlfw_Shutdown();
		ImGui::DestroyContext();
		
	}

	bool shouldCloseWindow()
	{
		return glfwWindowShouldClose(window);
	}

	void pollSwapWindow()
	{
		glfwPollEvents();
		glfwSwapBuffers(window);
	}
}