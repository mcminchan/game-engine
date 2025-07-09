#ifndef PROJECT1_BUFFERS_H
#define PROJECT1_BUFFERS_H

namespace Buffers
{
	void setupRenderFrame();
	void initEditFramebuffer();
	void initCameraFramebuffer();
	void updateRenderFrame();
	unsigned int genCameraUniformBuffer();
	unsigned int getEditFramebufferTex();
	unsigned int getCameraFramebufferTex();
	void setUniformBufferPoint(unsigned int buffer, unsigned int point,unsigned int range);
	void setFramebuffer(unsigned int& FBO, unsigned int& RBO, unsigned int& texture);
	void adjustFrame(unsigned int& FBO, unsigned int& RBO, unsigned int& texture);
	void initFrame(const unsigned int FBO);

	void deleteFramebuffers();
}
#endif