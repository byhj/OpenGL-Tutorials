#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <iostream>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <vector>

#include "common/controls.cpp"
#include "common/texture.cpp"
#include "common/objloader.cpp"
#include "common/shader.h"

const GLuint Width(1200), Height(800);     //window size
void key_callback(GLFWwindow *window, int key, int scancode, int action, int mode);
void init();
void render(GLFWwindow *window);

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
	glfwSetInputMode(window, GLFW_STICKY_KEYS, GL_TRUE);
	glfwSetCursorPos(window, 1200/2, 800/2);
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
		render(window);
		glfwSwapBuffers(window);
	}

	glfwTerminate();
	return 0;
}

Shader TriangleShader("Triangle Shader");
GLuint vao, uvbuffer, vertexbuffer, normalbuffer;
GLuint program, texture;
GLuint proj_loc, mv_loc, tex_loc;
GLuint VertexSize;

void init_buffer()
{
	// Read our .obj file
	std::vector<glm::vec3> vertices;
	std::vector<glm::vec2> uvs;
	std::vector<glm::vec3> normals; // Won't be used at the moment.
	bool res = loadOBJ("../../media/object/suzanne.obj", vertices, uvs, normals);

	glGenBuffers(1, &vertexbuffer);
	glBindBuffer(GL_ARRAY_BUFFER, vertexbuffer);
	glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(glm::vec3), &vertices[0], GL_STATIC_DRAW);

	glGenBuffers(1, &uvbuffer);
	glBindBuffer(GL_ARRAY_BUFFER, uvbuffer);
	glBufferData(GL_ARRAY_BUFFER, uvs.size() * sizeof(glm::vec2), &uvs[0], GL_STATIC_DRAW);

	glGenBuffers(1, &normalbuffer);
	glBindBuffer(GL_ARRAY_BUFFER, normalbuffer);
	glBufferData(GL_ARRAY_BUFFER, normals.size() * sizeof(glm::vec3), &normals[0], GL_STATIC_DRAW);
	
	VertexSize = vertices.size();
}

void init_shader()
{
	TriangleShader.init();
	TriangleShader.attach(GL_VERTEX_SHADER, "phong.vert");
	TriangleShader.attach(GL_FRAGMENT_SHADER, "phong.frag");
	TriangleShader.link();
	program = TriangleShader.GetProgram();

	mv_loc = glGetUniformLocation(program, "mv");
	proj_loc = glGetUniformLocation(program, "proj");
	tex_loc = glGetUniformLocation(program, "tex");
}

void init_vertexArray()
{
	glGenVertexArrays(1, &vao);
	glBindVertexArray(vao);

	glBindBuffer(GL_ARRAY_BUFFER, vertexbuffer);
	glEnableVertexAttribArray(0);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, 0);
	glBindBuffer(GL_ARRAY_BUFFER, 0);

	glBindBuffer(GL_ARRAY_BUFFER, normalbuffer);
	glEnableVertexAttribArray(1);
	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 0, 0);
	glBindBuffer(GL_ARRAY_BUFFER, 0);

	glEnableVertexAttribArray(2);
	glBindBuffer(GL_ARRAY_BUFFER, uvbuffer);
	glVertexAttribPointer( 2, 2, GL_FLOAT, GL_FALSE, 0, 0);                          

	glBindVertexArray(0);
	glDisableVertexAttribArray(0);
	glDisableVertexAttribArray(1);
	glDisableVertexAttribArray(2);
}

void init_texture()
{
	texture = loadDDS("../../media/texture/color.DDS");
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

void render(GLFWwindow *window)
{
	glClearColor(0.0f, 0.0f, 0.4f, 0.0f);
	glClearDepth(1.0f);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	computeMatricesFromInputs(window);
	glm::mat4 ProjectionMatrix = getProjectionMatrix();
	glm::mat4 ViewMatrix = getViewMatrix();
	glm::mat4 ModelMatrix = glm::mat4(1.0);
	glm::mat4 MV = ViewMatrix * ModelMatrix;

	glUseProgram(program);

	glUniformMatrix4fv(mv_loc, 1, GL_FALSE, &MV[0][0]);
	glUniformMatrix4fv(proj_loc, 1, GL_FALSE, &ProjectionMatrix[0][0]);
	glBindVertexArray(vao);
	glBindTexture(GL_TEXTURE_2D, texture);
	glDrawArrays(GL_TRIANGLES, 0, VertexSize);
	glBindVertexArray(0);
	glUseProgram(0);
}

void key_callback(GLFWwindow *window, int key, int scancode, int action, int mode)
{
	if (key == GLFW_KEY_ESCAPE && action == GLFW_PRESS)
		glfwSetWindowShouldClose(window, GL_TRUE);  //we should close the window
}