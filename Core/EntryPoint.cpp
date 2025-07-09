#include "Window.h"
#include "Buffers.h"
#include "Physics.h"
#include "Primitives.h"
#include "AssetManager.h"
#include "ShaderManager.h"

namespace Buffers
{
	extern void deleteFramebuffers();
}
namespace Camera
{
	extern void setup();
}

int main()
{

	/////////////////////glfw init
	Window::initWindow();

	Physics::initPhysics();
	initPrimitives();

	///////¼ÎÀÌ´õ »ý¼º
	

	ShaderManager::initShaders();
	AssetManager::initGameObjects();
	Buffers::setupRenderFrame();
	Camera::setup();

	extern void renderLoop();
	renderLoop();

	Window::terminateWindow();
	Physics::freePhysics();
	Buffers::deleteFramebuffers();
}