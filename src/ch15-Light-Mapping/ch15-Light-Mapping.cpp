// Include standard headers
#include <stdio.h>
#include <stdlib.h>
#include <vector>

// Include GLEW
#include <GL/glew.h>

// Include GLFW
#include <GLFW/glfw3.h>
GLFWwindow* window;

// Include GLM
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
using namespace glm;

#include <common/shader.h>
#include <common/texture.hpp>
#include <common/controls.hpp>
#include <common/objloader.hpp>

Shader LightMapShader("Light Map Shader");
GLuint program;
GLuint vao;
// Load it into a VBO
GLuint TextureID;
GLuint uvbuffer;
GLuint vertexbuffer;
GLuint Texture;
GLuint MatrixID;
// Read our .obj file
std::vector<glm::vec3> vertices;
std::vector<glm::vec2> uvs;
std::vector<glm::vec3> normals; // Won't be used at the moment.

void init_buffer();
void init_vertexArray();
void init_shader();
void init_texture();
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

void init()
{
	init_buffer();
	init_vertexArray();
	init_shader();
	init_texture();

	// Dark blue background
	glClearColor(0.0f, 0.0f, 0.4f, 0.0f);
	glClearDepth(1.0f);
	glEnable(GL_DEPTH_TEST);
	glDepthFunc(GL_LESS); 
	glEnable(GL_CULL_FACE);

}

void render(GLFWwindow *window)
{
	// Clear the screen
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	// Use our shader
	glUseProgram(program);
	glBindVertexArray(vao);

	// Compute the MVP matrix from keyboard and mouse input
	computeMatricesFromInputs(window);
	glm::mat4 ProjectionMatrix = getProjectionMatrix();
	glm::mat4 ViewMatrix = getViewMatrix();
	glm::mat4 ModelMatrix = glm::mat4(1.0);
	glm::mat4 MVP = ProjectionMatrix * ViewMatrix * ModelMatrix;

	glUniformMatrix4fv(MatrixID, 1, GL_FALSE, &MVP[0][0]);

	// Bind our texture in Texture Unit 0
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, Texture);
	glDrawArrays(GL_TRIANGLES, 0, vertices.size() );

}

void init_buffer()
{

	bool res = loadOBJ("../../media/object/room.obj", vertices, uvs, normals);


	glGenBuffers(1, &vertexbuffer);
	glBindBuffer(GL_ARRAY_BUFFER, vertexbuffer);
	glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(glm::vec3), &vertices[0], GL_STATIC_DRAW);
	glBindBuffer(GL_ARRAY_BUFFER, 0);

	glGenBuffers(1, &uvbuffer);
	glBindBuffer(GL_ARRAY_BUFFER, uvbuffer);
	glBufferData(GL_ARRAY_BUFFER, uvs.size() * sizeof(glm::vec2), &uvs[0], GL_STATIC_DRAW);
	glBindBuffer(GL_ARRAY_BUFFER, 0);
		

}

void shutdown()
{
	// Cleanup VBO and shader
	glDeleteBuffers(1, &vertexbuffer);
	glDeleteBuffers(1, &uvbuffer);
	glDeleteProgram(program);
	glDeleteTextures(1, &Texture);
	glDeleteVertexArrays(1, &vao);

}

void init_shader()
{
	LightMapShader.init();
	LightMapShader.attach(GL_VERTEX_SHADER, "lightmap.vert");
	LightMapShader.attach(GL_FRAGMENT_SHADER, "lightmap.frag");
	LightMapShader.link();
	LightMapShader.interfaceInfo();
    LightMapShader.use();
	program = LightMapShader.GetProgram();
	MatrixID = glGetUniformLocation(program, "MVP");
	TextureID  = glGetUniformLocation(program, "tex");
	glUniform1i(TextureID, 0);
}

void init_vertexArray()
{
	glGenVertexArrays(1, &vao);
	glBindVertexArray(vao);

	glBindBuffer(GL_ARRAY_BUFFER, vertexbuffer);
	// 1rst attribute buffer : vertices
	glEnableVertexAttribArray(0);
	glVertexAttribPointer(
		0,                  // attribute
		3,                  // size
		GL_FLOAT,           // type
		GL_FALSE,           // normalized?
		0,                  // stride
		(void*)0            // array buffer offset
		);


	glBindBuffer(GL_ARRAY_BUFFER, uvbuffer);
	// 2nd attribute buffer : UVs
	glEnableVertexAttribArray(1);
	glVertexAttribPointer(
		1,                                // attribute
		2,                                // size
		GL_FLOAT,                         // type
		GL_FALSE,                         // normalized?
		0,                                // stride
		(void*)0                          // array buffer offset
		);

	//glDisableVertexAttribArray(0);
	//glDisableVertexAttribArray(1);
}

void init_texture()
{
	// Load the texture
	Texture = loadDDS("../../media/texture/lightmap.DDS");
}

void key_callback(GLFWwindow *window, int key, int scancode, int action, int mode)
{
	if (key == GLFW_KEY_ESCAPE && action == GLFW_PRESS)
		glfwSetWindowShouldClose(window, GL_TRUE);  //we should close the window
}