#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <iostream>

#include "common/shader.h"

const GLuint Width(1000), Height(800);     //window size
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
GLuint vao, vbo;
GLuint program;

static const GLfloat VertexData[] = 
{
	-1.0f, -1.0f, 0.0f,
	 1.0f, -1.0f, 0.0f,
	 0.0f,  1.0f, 0.0f
};

void init_buffer()
{
	glGenBuffers(1, &vbo);
	glBindBuffer(GL_ARRAY_BUFFER, vbo);
	glBufferData(GL_ARRAY_BUFFER, sizeof(VertexData), VertexData, GL_STATIC_DRAW);
	glBindBuffer(GL_ARRAY_BUFFER, 0);
}

void init_shader()
{
	TriangleShader.init();
	TriangleShader.attach(GL_VERTEX_SHADER, "triangle.vert");
	TriangleShader.attach(GL_FRAGMENT_SHADER, "triangle.frag");
	TriangleShader.link();
	program = TriangleShader.GetProgram();
}

void init_vertexArray()
{
	glGenVertexArrays(1, &vao);
	glBindVertexArray(vao);
	glBindBuffer(GL_ARRAY_BUFFER, vbo);

	glEnableVertexAttribArray(0);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, 0);

	glBindVertexArray(0);
}

void init()
{
	glClearColor(0.0f, 0.0f, 0.4f, 0.0f);

	init_shader();
	init_buffer();
	init_vertexArray();
}

void render()
{

	glClear(GL_COLOR_BUFFER_BIT);
	glUseProgram(program);
	glBindVertexArray(vao);

	glDrawArrays(GL_TRIANGLES, 0, 3);

	glBindVertexArray(0);
	glUseProgram(0);

}

void key_callback(GLFWwindow *window, int key, int scancode, int action, int mode)
{
	if (key == GLFW_KEY_ESCAPE && action == GLFW_PRESS)
		glfwSetWindowShouldClose(window, GL_TRUE);  //we should close the window
}