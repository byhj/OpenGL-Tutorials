#include <GL/glew.h>
#include <GL/glfw3.h>
#include <iostream>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

#include "common/controls.cpp"
#include "common/texture.cpp"
#include "common/shader.h"

const GLuint Width(1200), Height(800);     //window size
void key_callback(GLFWwindow *window, int key, int scancode, int action, int mode);
void init();
void render();

int main()
{
	std::cout << "Starting GLFW context, OpenGL3.3" << std::endl;
	glfwInit();
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3); //opengl 3.3
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE); //using opengl core file
	glfwWindowHint(GLFW_RESIZABLE, GL_FALSE);

	// Create a GLFWwindow object that we can use for GLFW's functions
	GLFWwindow *window = glfwCreateWindow(Width, Height, "OpenGL Tutorial", nullptr, nullptr);
	glfwSetWindowPos(window, 300, 50);
	glfwMakeContextCurrent(window);
	if (window == NULL)
	{
		std::cerr << "Failed to create GLFW window" << std::endl;
		glfwTerminate();
		return -1;
	}

	glfwSetKeyCallback(window, key_callback);  //keyboard function
	glewExperimental = GL_TRUE;

	// Initialize GLEW to setup the OpenGL Function pointers
	if (glewInit() != GLEW_OK)
	{
		std::cerr << "Failed to initializz GLEW" << std::endl;
		return -1;
	}
	init();

	glViewport(0, 0, Width, Height);
	while (!glfwWindowShouldClose(window))
	{
		glfwPollEvents();
		render();
		glfwSwapBuffers(window);
	}

	glfwTerminate();
	return 0;
}

Shader TriangleShader("Triangle Shader");
GLuint vao, vbo, vbo_color;
GLuint program, texture;
GLuint mvp_loc, tex_loc;

static const GLfloat VertexData[] = {
	-1.0f,-1.0f,-1.0f,
	-1.0f,-1.0f, 1.0f,
	-1.0f, 1.0f, 1.0f,
	1.0f, 1.0f,-1.0f,
	-1.0f,-1.0f,-1.0f,
	-1.0f, 1.0f,-1.0f,
	1.0f,-1.0f, 1.0f,
	-1.0f,-1.0f,-1.0f,
	1.0f,-1.0f,-1.0f,
	1.0f, 1.0f,-1.0f,
	1.0f,-1.0f,-1.0f,
	-1.0f,-1.0f,-1.0f,
	-1.0f,-1.0f,-1.0f,
	-1.0f, 1.0f, 1.0f,
	-1.0f, 1.0f,-1.0f,
	1.0f,-1.0f, 1.0f,
	-1.0f,-1.0f, 1.0f,
	-1.0f,-1.0f,-1.0f,
	-1.0f, 1.0f, 1.0f,
	-1.0f,-1.0f, 1.0f,
	1.0f,-1.0f, 1.0f,
	1.0f, 1.0f, 1.0f,
	1.0f,-1.0f,-1.0f,
	1.0f, 1.0f,-1.0f,
	1.0f,-1.0f,-1.0f,
	1.0f, 1.0f, 1.0f,
	1.0f,-1.0f, 1.0f,
	1.0f, 1.0f, 1.0f,
	1.0f, 1.0f,-1.0f,
	-1.0f, 1.0f,-1.0f,
	1.0f, 1.0f, 1.0f,
	-1.0f, 1.0f,-1.0f,
	-1.0f, 1.0f, 1.0f,
	1.0f, 1.0f, 1.0f,
	-1.0f, 1.0f, 1.0f,
	1.0f,-1.0f, 1.0f
};

static const GLfloat uvData[] = {
	0.000059f, 0.000004f, 
	0.000103f, 0.336048f, 
	0.335973f, 0.335903f, 
	1.000023f, 0.000013f, 
	0.667979f, 0.335851f, 
	0.999958f, 0.336064f, 
	0.667979f, 0.335851f, 
	0.336024f, 0.671877f, 
	0.667969f, 0.671889f, 
	1.000023f, 0.000013f, 
	0.668104f, 0.000013f, 
	0.667979f, 0.335851f, 
	0.000059f, 0.000004f, 
	0.335973f, 0.335903f, 
	0.336098f, 0.000071f, 
	0.667979f, 0.335851f, 
	0.335973f, 0.335903f, 
	0.336024f, 0.671877f, 
	1.000004f, 0.671847f, 
	0.999958f, 0.336064f, 
	0.667979f, 0.335851f, 
	0.668104f, 0.000013f, 
	0.335973f, 0.335903f, 
	0.667979f, 0.335851f, 
	0.335973f, 0.335903f, 
	0.668104f, 0.000013f, 
	0.336098f, 0.000071f, 
	0.000103f, 0.336048f, 
	0.000004f, 0.671870f, 
	0.336024f, 0.671877f, 
	0.000103f, 0.336048f, 
	0.336024f, 0.671877f, 
	0.335973f, 0.335903f, 
	0.667969f, 0.671889f, 
	1.000004f, 0.671847f, 
	0.667979f, 0.335851f
};

void init_buffer()
{
	glGenBuffers(1, &vbo);
	glBindBuffer(GL_ARRAY_BUFFER, vbo);
	glBufferData(GL_ARRAY_BUFFER, sizeof(VertexData), VertexData, GL_STATIC_DRAW);
	glBindBuffer(GL_ARRAY_BUFFER, 0);

	glGenBuffers(1, &vbo_color);
	glBindBuffer(GL_ARRAY_BUFFER, vbo_color);
	glBufferData(GL_ARRAY_BUFFER, sizeof(uvData), uvData, GL_STATIC_DRAW);
	glBindBuffer(GL_ARRAY_BUFFER, 0);
}

void init_shader()
{
	TriangleShader.init();
	TriangleShader.attach(GL_VERTEX_SHADER, "cube.vert");
	TriangleShader.attach(GL_FRAGMENT_SHADER, "cube.frag");
	TriangleShader.link();
	program = TriangleShader.GetProgram();

	mvp_loc = glGetUniformLocation(program, "mvp");
	tex_loc = glGetUniformLocation(program, "tex");
}

void init_vertexArray()
{
	glGenVertexArrays(1, &vao);
	glBindVertexArray(vao);

	glBindBuffer(GL_ARRAY_BUFFER, vbo);
	glEnableVertexAttribArray(0);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, 0);
	glBindBuffer(GL_ARRAY_BUFFER, 0);

	glBindBuffer(GL_ARRAY_BUFFER, vbo_color);
	glEnableVertexAttribArray(1);
	glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 0, 0);
	glBindBuffer(GL_ARRAY_BUFFER, 0);

	glBindVertexArray(0);
}

void init_texture()
{
	texture = loadDDS("../../media/texture/uvtemplate.DDS");
}

void init()
{
	glEnable(GL_DEPTH_TEST);
    glDepthFunc(GL_LESS);
	init_texture();
	init_shader();
	init_buffer();
	init_vertexArray();
}



void render()
{
	glClearColor(0.0f, 0.0f, 0.4f, 0.0f);
	glClearDepth(1.0f);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	float time = static_cast<GLfloat>( glfwGetTime() );

	glm::mat4 model = glm::rotate(glm::mat4(1.0f), time, glm::vec3(1.0f, 1.0f, 1.0f) );
	glm::mat4 view = glm::lookAt(glm::vec3(0.0f, 0.0f, 5.0f), glm::vec3(0.0f, 0.0f, 0.0f),
		glm::vec3(0.0f, 1.0f, 0.0f) );
	glm::mat4 proj = glm::perspective(45.0f, float(Width) / Height, 0.1f, 1000.0f);
	glm::mat4 mvp = proj * view * model;

	glUseProgram(program);

	//Use set data function after use shader program;
	glBindTexture(GL_TEXTURE_2D, texture);
	glUniformMatrix4fv(mvp_loc, 1, GL_FALSE, &mvp[0][0]);
	glBindVertexArray(vao);
	glDrawArrays(GL_TRIANGLES, 0, 36);
	glBindVertexArray(0);
	glUseProgram(0);
}

void key_callback(GLFWwindow *window, int key, int scancode, int action, int mode)
{
	if (key == GLFW_KEY_ESCAPE && action == GLFW_PRESS)
		glfwSetWindowShouldClose(window, GL_TRUE);  //we should close the window
}