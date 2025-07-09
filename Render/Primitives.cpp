#include <glad/glad.h>
#include <iostream>


static unsigned int cubeVAO{ 0 };	//전역변수는 기본적으로 extern이고 static을 써야 이 translation unit 내에서만 사용 가능하다
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

void initPrimitives()		//함수는 기본적으로 extern이고 extern 키워드를 써도 무시된다
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
					const unsigned int vertSize, const unsigned int* indices, const unsigned int indSize)		//initCube는 클래스의 private처럼 initPrimitive에서 호출하기 때문에 여기서만 쓸거다.
{
	
	glGenVertexArrays(1, &VAO);
	glGenBuffers(1, &VBO);
	glGenBuffers(1, &EBO);

	glBindBuffer(GL_ARRAY_BUFFER, VBO);
	glBufferData(GL_ARRAY_BUFFER,vertSize, vertices, GL_STATIC_DRAW);

	glBindVertexArray(VAO);			//VAO는 glVertexAttribPointer를 호출할 때 GL_VERTEX_BUFFER에 bind되어있는 VBO를 저장함.(bind할떄 저장 X)

	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);		//glBindBuffer를 호출할 때 GL_ELEMENT_ARRAY_BUFFER에 bind하는 경우 그 bind하는 EBO를 저장함.
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