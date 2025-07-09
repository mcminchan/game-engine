#include <glad/glad.h>
#include <iostream>


static unsigned int cubeVAO{ 0 };	//���������� �⺻������ extern�̰� static�� ��� �� translation unit �������� ��� �����ϴ�
static unsigned int cubeVBO{ 0 };
static unsigned int cubeEBO{ 0 };
static unsigned int cubeEBOsize{ 0 };
static unsigned int rectVAO{ 0 };
static unsigned int rectVBO{ 0 };
static unsigned int rectEBO{ 0 };
static unsigned int rectEBOsize{ 0 };

static void initRect();
static void initCube();
static void setupVAO(unsigned int& VAO, unsigned int& VBO, unsigned int& EBO, const float* vertices,
	const unsigned int vertSize, const unsigned int* indices, const unsigned int indSize);

void initPrimitives()		//�Լ��� �⺻������ extern�̰� extern Ű���带 �ᵵ ���õȴ�
{
	
	initCube();

	initRect();
}

static void initCube()
{
	const float cubeVert[]{ -1.0f,-1.0f,1.0f			,0.0f,0.0f,1.0f		,0.0f,0.0f
					 ,1.0f,-1.0f,1.0f			,0.0f,0.0f,1.0f		,1.0f,0.0f
					 ,1.0f, 1.0f,1.0f			,0.0f,0.0f,1.0f		,1.0f,1.0f
					 ,-1.0f,1.0f,1.0f			,0.0f,0.0f,1.0f		,0.0f,1.0f

					,-1.0f,-1.0f,-1.0f			,0.0f,0.0f,-1.0f	,0.0f,0.0f
					,1.0f,-1.0f,-1.0f			,0.0f,0.0f,-1.0f	,1.0f,0.0f
					,1.0f, 1.0f,-1.0f			,0.0f,0.0f,-1.0f	,1.0f,1.0f
					,-1.0f,1.0f,-1.0f			,0.0f,0.0f,-1.0f	,0.0f,1.0f

					,-1.0f,1.0f,1.0f			,0.0f,1.0f,0.0f		,0.0f,1.0f
					,1.0f,1.0f,1.0f				,0.0f,1.0f,0.0f		,1.0f,1.0f
					,1.0f, 1.0f,-1.0f			,0.0f,1.0f,0.0f		,1.0f,0.0f
					,-1.0f,1.0f,-1.0f			,0.0f,1.0f,0.0f		,0.0f,0.0f

					,-1.0f,-1.0f,1.0f			,0.0f,-1.0f,0.0f	,0.0f,1.0f
					,1.0f,-1.0f,1.0f			,0.0f,-1.0f,0.0f	,1.0f,1.0f
					,1.0f, -1.0f,-1.0f			,0.0f,-1.0f,0.0f	,1.0f,0.0f
					,-1.0f,-1.0f,-1.0f			,0.0f,-1.0f,0.0f	,0.0f,0.0f

					,-1.0f,-1.0f,-1.0f			,-1.0f,0.0f,0.0f	,0.0f,0.0f
					,-1.0f,-1.0f,1.0f			,-1.0f,0.0f,0.0f	,0.0f,1.0f
					,-1.0f, 1.0f,1.0f			,-1.0f,0.0f,0.0f	,1.0f,1.0f
					,-1.0f,1.0f,-1.0f			,-1.0f,0.0f,0.0f	,1.0f,0.0f

					,1.0f,-1.0f,1.0f			,1.0f,0.0f,0.0f		,0.0f,1.0f
					,1.0f,-1.0f,-1.0f			,1.0f,0.0f,0.0f		,0.0f,0.0f
					,1.0f,1.0f,-1.0f			,1.0f,0.0f,0.0f		,1.0f,0.0f
					,1.0f, 1.0f,1.0f			,1.0f,0.0f,0.0f		,1.0f,1.0f };
	const unsigned int cubeIndices[]{ 0, 1,2,2,3,0
								,4,6,5,4,7,6
								,8,9,10,10,11,8
								,12,14,13,12,15,14
								,16,17,18,18,19,16
								,20,21,22,22,23,20 };
	cubeEBOsize = sizeof(cubeIndices) / sizeof(unsigned int);
	setupVAO(cubeVAO, cubeVBO, cubeEBO, cubeVert, sizeof(cubeVert), cubeIndices, sizeof(cubeIndices));
}
unsigned int getCubeVAO()
{
	return cubeVAO;
}

unsigned int getCubeEBOsize()
{
	return cubeEBOsize;
}


static void initRect()
{
	const float quadVert[]{ -1.0f, -1.0f,0.0f			,0.0f,0.0f,1.0f		,0.0f,0.0f
					 ,1.0f,-1.0f,0.0f			,0.0f,0.0f,1.0f		,1.0f,0.0f
					 ,1.0f,1.0f,0.0f			,0.0f,0.0f,1.0f		,1.0f,1.0f
					 ,-1.0f,1.0f,0.0f			,0.0f,0.0f,1.0f		,0.0f,1.0f };
	const unsigned int quadIndices[]{ 0, 1, 2, 0, 2, 3 };
	
	rectEBOsize = sizeof(quadIndices) / sizeof(unsigned int);
	setupVAO(rectVAO, rectVBO, rectEBO, quadVert, sizeof(quadVert), quadIndices, sizeof(quadIndices));
}

unsigned int getRectVAO()
{
	return rectVAO;
}

unsigned int getRectEBOsize()
{
	return rectEBOsize;
}

static void setupVAO(unsigned int& VAO, unsigned int& VBO, unsigned int& EBO, const float* vertices,
					const unsigned int vertSize, const unsigned int* indices, const unsigned int indSize)		//initCube�� Ŭ������ privateó�� initPrimitive���� ȣ���ϱ� ������ ���⼭�� ���Ŵ�.
{
	
	glGenVertexArrays(1, &VAO);
	glGenBuffers(1, &VBO);
	glGenBuffers(1, &EBO);

	glBindBuffer(GL_ARRAY_BUFFER, VBO);
	glBufferData(GL_ARRAY_BUFFER,vertSize, vertices, GL_STATIC_DRAW);

	glBindVertexArray(VAO);			//VAO�� glVertexAttribPointer�� ȣ���� �� GL_VERTEX_BUFFER�� bind�Ǿ��ִ� VBO�� ������.(bind�ҋ� ���� X)

	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);		//glBindBuffer�� ȣ���� �� GL_ELEMENT_ARRAY_BUFFER�� bind�ϴ� ��� �� bind�ϴ� EBO�� ������.
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, indSize, indices, GL_STATIC_DRAW);

	glVertexAttribPointer(0, 3, GL_FLOAT, false, 8 * sizeof(float), 0);
	glVertexAttribPointer(1, 3, GL_FLOAT, false, 8 * sizeof(float), (void*)(3 * sizeof(float)));
	glVertexAttribPointer(2, 2, GL_FLOAT, false, 8 * sizeof(float), (void*)(6 * sizeof(float)));
	glEnableVertexAttribArray(0);
	glEnableVertexAttribArray(1);
	glEnableVertexAttribArray(2);
}

void renderCube()
{
	glBindVertexArray(cubeVAO);
	glDrawElements(GL_TRIANGLES, cubeEBOsize, GL_UNSIGNED_INT, 0);
}