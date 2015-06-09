#include <GL/glew.h>
#include <GL/glfw3.h>
#include <iostream>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <vector>

#include "common/controls.hpp"
#include "common/texture.hpp"
#include "common/objloader.hpp"
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
GLuint vao, uvbuffer, vertexbuffer, normalbuffer, elementbuffer;
GLuint program, texture;
GLuint proj_loc, mv_loc, tex_loc;
GLuint VertexSize;
GLuint fbo, tbo, rbo, dtbo;

Shader FboShader("Frame Buffer");
GLuint f_vao, f_uv, f_vbo ;
GLuint f_program, f_tex_loc;
static const GLfloat QuadData[] = 
{
	-1.0f, -1.0f, 0.0f, 0.0f, 0.0f,
	-0.5f, -1.0f, 0.0f, 1.0f, 0.0f,
    -0.5f, -0.5f, 0.0f, 1.0f, 1.0f,

	-0.5f, -0.5f, 0.0f, 1.0f, 1.0f,
	-1.0f, -0.5f, 0.0f, 0.0f, 1.0f,
	-1.0f, -1.0f, 0.0f , 0.0f, 0.0f
};

void init_framebuffer()
{
	glGenFramebuffers(1, &fbo);
	glBindFramebuffer(GL_FRAMEBUFFER, fbo);

	glGenTextures(1, &tbo);
	glBindTexture(GL_TEXTURE_2D, tbo);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, 1200, 800, 0, GL_RGB, GL_UNSIGNED_BYTE, 0);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST); 
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
	glFramebufferTexture(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, tbo, 0);

	if(glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
		std::cout << "Fbo is not complete" << std::endl;

	glGenTextures(1, &dtbo);
	glBindTexture(GL_TEXTURE_2D, dtbo);
	glTexImage2D(GL_TEXTURE_2D, 0,GL_DEPTH_COMPONENT32, 1200, 800, 0,GL_DEPTH_COMPONENT, GL_FLOAT, 0);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
	glFramebufferTexture(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, dtbo, 0);

	/*
	glGenRenderbuffers(1, &rbo);
	glBindRenderbuffer(GL_RENDERBUFFER, rbo);
	glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH_COMPONENT, 1200, 800);
	glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_RENDERBUFFER, rbo);
	
	*/
	GLenum drawbuffers[1] = {GL_COLOR_ATTACHMENT0};
	glDrawBuffers(1, drawbuffers);
}
void init_buffer()
{
	std::vector<unsigned short> indices;
	std::vector<glm::vec3> indexed_vertices;
	std::vector<glm::vec2> indexed_uvs;
	std::vector<glm::vec3> indexed_normals;
	bool res = loadAssImp("../../media/object/suzanne.obj", indices, indexed_vertices, indexed_uvs, indexed_normals);
	

	glGenBuffers(1, &vertexbuffer);
	glBindBuffer(GL_ARRAY_BUFFER, vertexbuffer);
	glBufferData(GL_ARRAY_BUFFER, indexed_vertices.size() * sizeof(glm::vec3), &indexed_vertices[0], GL_STATIC_DRAW);


	glGenBuffers(1, &uvbuffer);
	glBindBuffer(GL_ARRAY_BUFFER, uvbuffer);
	glBufferData(GL_ARRAY_BUFFER, indexed_uvs.size() * sizeof(glm::vec2), &indexed_uvs[0], GL_STATIC_DRAW);


	glGenBuffers(1, &normalbuffer);
	glBindBuffer(GL_ARRAY_BUFFER, normalbuffer);
	glBufferData(GL_ARRAY_BUFFER, indexed_normals.size() * sizeof(glm::vec3), &indexed_normals[0], GL_STATIC_DRAW);


	glGenBuffers(1, &elementbuffer);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, elementbuffer);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, indices.size() * sizeof(unsigned short), &indices[0] , GL_STATIC_DRAW);

	VertexSize = indices.size();

	glGenBuffers(1, &f_vbo);
	glBindBuffer(GL_ARRAY_BUFFER, f_vbo);
	glBufferData(GL_ARRAY_BUFFER, sizeof(QuadData), QuadData, GL_STATIC_DRAW);

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


	FboShader.init();
	FboShader.attach(GL_VERTEX_SHADER, "fbo.vert");
	FboShader.attach(GL_FRAGMENT_SHADER, "fbo.frag");
	FboShader.link();
	f_program = FboShader.GetProgram();
	f_tex_loc = glGetUniformLocation(f_program, "tex");
	glUniform1i(f_tex_loc, 0);
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

    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, elementbuffer);
	glBindVertexArray(0);
	glDisableVertexAttribArray(0);
	glDisableVertexAttribArray(1);
	glDisableVertexAttribArray(2);

	glGenVertexArrays(1, &f_vao);
	glBindVertexArray(f_vao);
	glBindBuffer(GL_ARRAY_BUFFER, f_vbo);
	glEnableVertexAttribArray(0);
	glEnableVertexAttribArray(1);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 5 * sizeof(GLfloat), 0);
	glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 5 * sizeof(GLfloat), (GLvoid*) (3 * sizeof(GLfloat)) );
	glBindVertexArray(0);
	glDisableVertexAttribArray(0);
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
	init_framebuffer();
}

void render(GLFWwindow *window)
{

	//First We render to fbo
	glUseProgram(program);
	glBindFramebuffer(GL_FRAMEBUFFER, fbo);
	glClearColor(0.0f, 0.0f, 0.0f, 0.0f);
	glClearDepth(1.0f);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	glViewport(0,0,1200, 800); // Render on the whole framebuffer, complete from the lower left corner to the upper right

	computeMatricesFromInputs(window);
	glm::mat4 ProjectionMatrix = getProjectionMatrix();
	glm::mat4 ViewMatrix = getViewMatrix();
	glm::mat4 ModelMatrix = glm::mat4(1.0);
	glm::mat4 MV = ViewMatrix * ModelMatrix;

	glUniformMatrix4fv(mv_loc, 1, GL_FALSE, &MV[0][0]);
	glUniformMatrix4fv(proj_loc, 1, GL_FALSE, &ProjectionMatrix[0][0]);
	glBindVertexArray(vao);
	glBindTexture(GL_TEXTURE_2D, texture);
	// Draw the triangles !
	glDrawElements(
		GL_TRIANGLES,      // mode
		VertexSize,   // count
		GL_UNSIGNED_SHORT,   // type
		(void*)0           // element array buffer offset
		);

	glBindVertexArray(0);
	glUseProgram(0);
	glUseProgram(program);
	glBindFramebuffer(GL_FRAMEBUFFER, 0);
	glClearColor(0.0f, 0.0f, 0.4f, 0.0f);
	glClearDepth(1.0f);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	glViewport(0,0,1200, 800); // Render on the whole framebuffer, complete from the lower left corner to the upper right
	glUniformMatrix4fv(mv_loc, 1, GL_FALSE, &MV[0][0]);
	glUniformMatrix4fv(proj_loc, 1, GL_FALSE, &ProjectionMatrix[0][0]);
	glBindVertexArray(vao);
	glBindTexture(GL_TEXTURE_2D, texture);
	// Draw the triangles !
	glDrawElements(
		GL_TRIANGLES,      // mode
		VertexSize,   // count
		GL_UNSIGNED_SHORT,   // type
		(void*)0           // element array buffer offset
		);
	glBindVertexArray(0);
	glUseProgram(0);


	glUseProgram(f_program);
	glBindFramebuffer(GL_FRAMEBUFFER, 0);
	glViewport(0, 0, 1200, 800); 
	glBindTexture(GL_TEXTURE_2D, tbo);
	glBindVertexArray(f_vao);
	glDrawArrays(GL_TRIANGLES, 0, 6);
	glBindVertexArray(0);
	glUseProgram(0);

}

void key_callback(GLFWwindow *window, int key, int scancode, int action, int mode)
{
	if (key == GLFW_KEY_ESCAPE && action == GLFW_PRESS)
		glfwSetWindowShouldClose(window, GL_TRUE);  //we should close the window
}