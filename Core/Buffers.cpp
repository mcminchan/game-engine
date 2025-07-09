#include "WindowConst.h"
#include "Buffers.h"
#include <glad/glad.h>
#include <glm/glm.hpp>


namespace Buffers
{
	static const int MAT4_SIZE = 64;
	static const int VEC_SIZE = 16;

	static unsigned int editFramebufferFBO;
	static unsigned int cameraFramebufferFBO;
	static unsigned int editFramebufferRBO;
	static unsigned int cameraFramebufferRBO;
	static unsigned int editFramebufferTex;
	static unsigned int cameraFramebufferTex;

	void setupRenderFrame()
	{
		setFramebuffer(editFramebufferFBO, editFramebufferRBO, editFramebufferTex);
		setFramebuffer(cameraFramebufferFBO, cameraFramebufferRBO, cameraFramebufferTex);
	}

	void initEditFramebuffer()
	{
		initFrame(editFramebufferFBO);
	}

	void initCameraFramebuffer()
	{
		initFrame(cameraFramebufferFBO);
	}

	void updateRenderFrame()
	{
		//callback function for window resize
		adjustFrame(editFramebufferFBO, editFramebufferRBO, editFramebufferTex);
		adjustFrame(cameraFramebufferFBO, cameraFramebufferRBO, cameraFramebufferTex);
	}
	unsigned int genCameraUniformBuffer()
	{

		unsigned int uniformBuffer;
		glGenBuffers(1, &uniformBuffer);
		glBindBuffer(GL_UNIFORM_BUFFER, uniformBuffer);
		glBufferData(GL_UNIFORM_BUFFER, MAT4_SIZE * 3 + VEC_SIZE, 0, GL_STATIC_DRAW);
		glBindBuffer(GL_UNIFORM_BUFFER, 0);
		return uniformBuffer;
	}

	unsigned int getEditFramebufferTex()
	{
		return editFramebufferTex;
	}
	unsigned int getCameraFramebufferTex()
	{
		return cameraFramebufferTex;
	}

	void setUniformBufferPoint(unsigned int buffer, unsigned int point,unsigned int range)
	{
		glBindBuffer(GL_UNIFORM_BUFFER, buffer);
		glBindBufferRange(GL_UNIFORM_BUFFER, point, buffer, 0, range);
	}

	void setFramebuffer(unsigned int& FBO, unsigned int& RBO, unsigned int& texture)
	{
		glGenFramebuffers(1, &FBO);
		glGenTextures(1, &texture);
		glGenRenderbuffers(1, &RBO);

		glBindFramebuffer(GL_FRAMEBUFFER, FBO);

		glBindTexture(GL_TEXTURE_2D, texture);
		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, Window::renderWidth, Window::renderHeight / 2, 0, GL_RGB, GL_UNSIGNED_BYTE, 0);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
		glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, texture, 0);

		glBindRenderbuffer(GL_RENDERBUFFER, RBO);
		glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH24_STENCIL8, Window::renderWidth, Window::renderHeight / 2);
		glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_STENCIL_ATTACHMENT, GL_RENDERBUFFER, RBO);
	}
	void adjustFrame(unsigned int& FBO, unsigned int& RBO, unsigned int& texture)
	{
		//adjust frame size fit into window render size
		glBindFramebuffer(GL_FRAMEBUFFER, FBO);

		glBindTexture(GL_TEXTURE_2D, texture);
		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, Window::renderWidth, Window::renderHeight / 2, 0, GL_RGB, GL_UNSIGNED_BYTE, 0);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
		glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, texture, 0);

		glBindRenderbuffer(GL_RENDERBUFFER, RBO);
		glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH24_STENCIL8, Window::renderWidth, Window::renderHeight / 2);
		glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_STENCIL_ATTACHMENT, GL_RENDERBUFFER, RBO);
	}

	void initFrame(const unsigned int FBO)
	{
		glBindFramebuffer(GL_FRAMEBUFFER, FBO);

		glEnable(GL_STENCIL_TEST);
		glEnable(GL_DEPTH_TEST);
		glEnable(GL_BLEND);

		glClearColor(0.1f, 0.1f, 0.1f, 1);	//glClearDepth glClearStencil 둘다 glClear에서 초기값 결정
		glClear(GL_COLOR_BUFFER_BIT | GL_STENCIL_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
		glStencilFunc(GL_ALWAYS, 0, 0xff);	//두번째 인자와 첫번째 인자(함수)로 비교해서 glStencilOp함수 설정값에 따라 
		//stencil buffer에 값을 씀, 비교할때 세번쨰 인자를 두번쨰 인자와 버퍼값 각각 and연산 후 비교
		glStencilMask(0x00);
	}


	void deleteFramebuffers()
	{
		glDeleteFramebuffers(1, &editFramebufferFBO);
		glDeleteRenderbuffers(1, &editFramebufferRBO);
		glDeleteTextures(1, &editFramebufferTex);
		glDeleteFramebuffers(1, &cameraFramebufferFBO);
		glDeleteRenderbuffers(1, &cameraFramebufferRBO);
		glDeleteTextures(1, &cameraFramebufferTex);
	}
}