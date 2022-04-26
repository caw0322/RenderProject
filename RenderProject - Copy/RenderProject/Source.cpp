#include <stdio.h>
#include <stdlib.h>
#include "ShapeGenerator.h"
#include "ShapeData.h"
#include <glad/glad.h>
#include <GLFW/glfw3.h>

#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <vector>
#include "shader.h"

#include <iostream>
#include "shader.hpp"
#include "texture.hpp"
#include "cylinder.h"

void framebuffer_size_callback(GLFWwindow* window, int width, int height);
void mouse_callback(GLFWwindow* window, double xpos, double ypos);
void scroll_callback(GLFWwindow* window, double xoffset, double yoffset);
void processInput(GLFWwindow* window);

// settings
const unsigned int SCR_WIDTH = 800;
const unsigned int SCR_HEIGHT = 600;
// offset variables for plane, sphere
const uint NUM_VERTICES_PER_TRI = 3;
const uint NUM_FLOATS_PER_VERTICE = 9;
const uint VERTEX_BYTE_SIZE = NUM_FLOATS_PER_VERTICE * sizeof(float);
// camera
glm::vec3 cameraPos = glm::vec3(0.0f, 0.0f, 3.0f);
glm::vec3 cameraFront = glm::vec3(0.0f, 0.0f, -1.0f);
glm::vec3 cameraUp = glm::vec3(0.0f, 1.0f, 0.0f);
bool changeView;

bool firstMouse = true;
float yaw = -90.0f;	// yaw is initialized to -90.0 degrees since a yaw of 0.0 results in a direction vector pointing to the right so we initially rotate a bit to the left.
float pitch = 0.0f;
float lastX = 800.0f / 2.0;
float lastY = 600.0 / 2.0;
float fov = 45.0f;

// timing
float deltaTime = 0.0f;	// time between current frame and last frame
float lastFrame = 0.0f;

float cameraSpeedChange = 2.5;
// variables for the plane
GLuint planeIndicies;
GLuint planeIndexOffset;

// variable for the sphere
GLuint sphereIndices;
GLuint sphereIndex;

int main()
{

	// glfw: initialize and configure
	// ------------------------------
	glfwInit();
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

#ifdef __APPLE__
	glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
#endif

	// glfw window creation
	// --------------------
	GLFWwindow* window = glfwCreateWindow(SCR_WIDTH, SCR_HEIGHT, "  ", NULL, NULL);
	if (window == NULL)
	{
		std::cout << "Failed to create GLFW window" << std::endl;
		glfwTerminate();
		return -1;
	}
	glfwMakeContextCurrent(window);
	glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);
	glfwSetCursorPosCallback(window, mouse_callback);
	glfwSetScrollCallback(window, scroll_callback);

	// tell GLFW to capture our mouse
	glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);

	// glad: load all OpenGL function pointers
	// ---------------------------------------
	if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
	{
		std::cout << "Failed to initialize GLAD" << std::endl;
		return -1;
	}

	// configure global opengl state
	// -----------------------------
	glEnable(GL_DEPTH_TEST);

	// build and compile our shader zprogram
	// ------------------------------------
	Shader ourShader("shaderfiles/7.3.camera.vs", "shaderfiles/7.3.camera.fs");


	// creating the plane
	ShapeData plane = ShapeGenerator::makePlane(10);

	unsigned int planeVBO{}, planeVAO;
	glGenVertexArrays(1, &planeVAO);
	glGenBuffers(1, &planeVBO);

	glBindVertexArray(planeVAO);
	glBindBuffer(GL_ARRAY_BUFFER, planeVBO);
	glBufferData(GL_ARRAY_BUFFER, plane.vertexBufferSize() + plane.indexBufferSize(), 0, GL_STATIC_DRAW);

	GLsizeiptr currentOffset = 0;

	glBufferSubData(GL_ARRAY_BUFFER, currentOffset, plane.vertexBufferSize(), plane.vertices);
	currentOffset += plane.vertexBufferSize();

	planeIndexOffset = currentOffset;

	glBufferSubData(GL_ARRAY_BUFFER, currentOffset, plane.indexBufferSize(), plane.indices);

	planeIndicies = plane.numIndices;

	glEnableVertexAttribArray(0);
	glEnableVertexAttribArray(1);
	glEnableVertexAttribArray(2);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, VERTEX_BYTE_SIZE, (void*)0);
	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, VERTEX_BYTE_SIZE, (void*)(sizeof(float) * 3));
	glVertexAttribPointer(2, 3, GL_FLOAT, GL_FALSE, VERTEX_BYTE_SIZE, (void*)(sizeof(float) * 6));
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, planeVBO);

	// creating the sphere
	ShapeData sphere = ShapeGenerator::makeSphere();

	unsigned int sphereVBO{}, sphereVAO;
	glGenVertexArrays(1, &sphereVAO);
	glGenBuffers(1, &sphereVBO);
	glBindVertexArray(sphereVAO);
	glBindBuffer(GL_ARRAY_BUFFER, sphereVBO);
	glBufferData(GL_ARRAY_BUFFER, sphere.vertexBufferSize() + sphere.indexBufferSize(), 0, GL_STATIC_DRAW);
	currentOffset = 0;
	glBufferSubData(GL_ARRAY_BUFFER, currentOffset, sphere.vertexBufferSize(), sphere.vertices);
	currentOffset += sphere.vertexBufferSize();
	sphereIndex = currentOffset;
	glBufferSubData(GL_ARRAY_BUFFER, currentOffset, sphere.indexBufferSize(), sphere.indices);
	sphereIndices = sphere.numIndices;
	glEnableVertexAttribArray(0);
	glEnableVertexAttribArray(1);
	glEnableVertexAttribArray(2);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, VERTEX_BYTE_SIZE, (void*)0);
	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, VERTEX_BYTE_SIZE, (void*)(sizeof(float) * 3));
	glVertexAttribPointer(2, 3, GL_FLOAT, GL_FALSE, VERTEX_BYTE_SIZE, (void*)(sizeof(float) * 6));
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, sphereVBO);
	// ------------------------------------------------------------------
	// creating the large square
	GLfloat square1[] = {
		-0.45f, -0.75f, -0.25f,  0.0f, 0.0f,
		 0.45f, -0.75f, -0.25f,  1.0f, 0.0f,
		 0.45f,  0.75f, -0.25f,  1.0f, 1.0f,
		 0.45f,  0.75f, -0.25f,  1.0f, 1.0f,
		-0.45f,  0.75f, -0.25f,  0.0f, 1.0f,
		-0.45f, -0.75f, -0.25f,  0.0f, 0.0f,

		-0.45f, -0.75f,  0.25f,  0.0f, 0.0f,
		 0.45f, -0.75f,  0.25f,  1.0f, 0.0f,
		 0.45f,  0.75f,  0.25f,  1.0f, 1.0f,
		 0.45f,  0.75f,  0.25f,  1.0f, 1.0f,
		-0.45f,  0.75f,  0.25f,  0.0f, 1.0f,
		-0.45f, -0.75f,  0.25f,  0.0f, 0.0f,

		-0.45f,  0.75f,  0.25f,  1.0f, 0.0f,
		-0.45f,  0.75f, -0.25f,  1.0f, 1.0f,
		-0.45f, -0.75f, -0.25f,  0.0f, 1.0f,
		-0.45f, -0.75f, -0.25f,  0.0f, 1.0f,
		-0.45f, -0.75f,  0.25f,  0.0f, 0.0f,
		-0.45f,  0.75f,  0.25f,  1.0f, 0.0f,

		 0.45f,  0.75f,  0.25f,  1.0f, 0.0f,
		 0.45f,  0.75f, -0.25f,  1.0f, 1.0f,
		 0.45f, -0.75f, -0.25f,  0.0f, 1.0f,
		 0.45f, -0.75f, -0.25f,  0.0f, 1.0f,
		 0.45f, -0.75f,  0.25f,  0.0f, 0.0f,
		 0.45f,  0.75f,  0.25f,  1.0f, 0.0f,

		-0.45f, -0.75f, -0.25f,  0.0f, 1.0f,
		 0.45f, -0.75f, -0.25f,  1.0f, 1.0f,
		 0.45f, -0.75f,  0.25f,  1.0f, 0.0f,
		 0.45f, -0.75f,  0.25f,  1.0f, 0.0f,
		-0.45f, -0.75f,  0.25f,  0.0f, 0.0f,
		-0.45f, -0.75f, -0.25f,  0.0f, 1.0f,

		-0.45f,  0.75f, -0.25f,  0.0f, 1.0f,
		 0.45f,  0.75f, -0.25f,  1.0f, 1.0f,
		 0.45f,  0.75f,  0.25f,  1.0f, 0.0f,
		 0.45f,  0.75f,  0.25f,  1.0f, 0.0f,
		-0.45f,  0.75f,  0.25f,  0.0f, 0.0f,
		-0.45f,  0.75f, -0.25f,  0.0f, 1.0f
	};

	unsigned int square1VBO, square1VAO;

	glGenVertexArrays(1, &square1VAO);
	glGenBuffers(1, &square1VBO);
	glBindVertexArray(square1VAO);
	glBindBuffer(GL_ARRAY_BUFFER, square1VBO);
	glBufferData(GL_ARRAY_BUFFER, sizeof(square1), square1, GL_STATIC_DRAW);



	// position attribute
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 5 * sizeof(GLfloat), (void*)0);
	glEnableVertexAttribArray(0);
	// texture coord attribute
	glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 5 * sizeof(GLfloat), (void*)(3 * sizeof(float)));
	glEnableVertexAttribArray(1);


	GLfloat square2[] = {
		-0.25f, -0.5f, -0.25f,  0.0f, 0.0f,
		 0.25f, -0.5f, -0.25f,  1.0f, 0.0f,
		 0.25f,  0.5f, -0.25f,  1.0f, 1.0f,
		 0.25f,  0.5f, -0.25f,  1.0f, 1.0f,
		-0.25f,  0.5f, -0.25f,  0.0f, 1.0f,
		-0.25f, -0.5f, -0.25f,  0.0f, 0.0f,

		-0.25f, -0.5f,  0.25f,  0.0f, 0.0f,
		 0.25f, -0.5f,  0.25f,  1.0f, 0.0f,
		 0.25f,  0.5f,  0.25f,  1.0f, 1.0f,
		 0.25f,  0.5f,  0.25f,  1.0f, 1.0f,
		-0.25f,  0.5f,  0.25f,  0.0f, 1.0f,
		-0.25f, -0.5f,  0.25f,  0.0f, 0.0f,

		-0.25f,  0.5f,  0.25f,  1.0f, 0.0f,
		-0.25f,  0.5f, -0.25f,  1.0f, 1.0f,
		-0.25f, -0.5f, -0.25f,  0.0f, 1.0f,
		-0.25f, -0.5f, -0.25f,  0.0f, 1.0f,
		-0.25f, -0.5f,  0.25f,  0.0f, 0.0f,
		-0.25f,  0.5f,  0.25f,  1.0f, 0.0f,

		 0.25f,  0.5f,  0.25f,  1.0f, 0.0f,
		 0.25f,  0.5f, -0.25f,  1.0f, 1.0f,
		 0.25f, -0.5f, -0.25f,  0.0f, 1.0f,
		 0.25f, -0.5f, -0.25f,  0.0f, 1.0f,
		 0.25f, -0.5f,  0.25f,  0.0f, 0.0f,
		 0.25f,  0.5f,  0.25f,  1.0f, 0.0f,

		-0.25f, -0.5f, -0.25f,  0.0f, 1.0f,
		 0.25f, -0.5f, -0.25f,  1.0f, 1.0f,
		 0.25f, -0.5f,  0.25f,  1.0f, 0.0f,
		 0.25f, -0.5f,  0.25f,  1.0f, 0.0f,
		-0.25f, -0.5f,  0.25f,  0.0f, 0.0f,
		-0.25f, -0.5f, -0.25f,  0.0f, 1.0f,

		-0.25f,  0.5f, -0.25f,  0.0f, 1.0f,
		 0.25f,  0.5f, -0.25f,  1.0f, 1.0f,
		 0.25f,  0.5f,  0.25f,  1.0f, 0.0f,
		 0.25f,  0.5f,  0.25f,  1.0f, 0.0f,
		-0.25f,  0.5f,  0.25f,  0.0f, 0.0f,
		-0.25f,  0.5f, -0.25f,  0.0f, 1.0f
	};

	unsigned int square2VBO, square2VAO;

	glGenVertexArrays(1, &square2VAO);
	glGenBuffers(1, &square2VBO);
	glBindVertexArray(square2VAO);
	glBindBuffer(GL_ARRAY_BUFFER, square2VBO);
	glBufferData(GL_ARRAY_BUFFER, sizeof(square2), square2, GL_STATIC_DRAW);

	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 5 * sizeof(GLfloat), (void*)0);
	glEnableVertexAttribArray(0);
	// texture coord attribute
	glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 5 * sizeof(GLfloat), (void*)(3 * sizeof(float)));
	glEnableVertexAttribArray(1);

	// cylinder buffer and array
	unsigned int cylVBO, cylVAO;
	glGenVertexArrays(1, &cylVAO);
	glGenBuffers(1, &cylVBO);
	glBindVertexArray(cylVAO);
	glBindBuffer(GL_ARRAY_BUFFER, cylVBO);


	// load and create a texture 
	// -------------------------
	unsigned int texture1, texture2, texture3, texture4, texture5;
	// texture 1
	// ---------
	glGenTextures(1, &texture1);
	glBindTexture(GL_TEXTURE_2D, texture1);
	// set the texture wrapping parameters
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
	// set texture filtering parameters
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	// load image, create texture and generate mipmaps
	int width, height, nrChannels;
	stbi_set_flip_vertically_on_load(true); // tell stb_image.h to flip loaded texture's on the y-axis.
	unsigned char* data = stbi_load("redleather.jpg", &width, &height, &nrChannels, 0);
	if (data)
	{
		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, data);
		glGenerateMipmap(GL_TEXTURE_2D);
	}
	else
	{
		std::cout << "Failed to load texture" << std::endl;
	}
	stbi_image_free(data);
	// texture 2
	// ---------
	glGenTextures(1, &texture2);
	glBindTexture(GL_TEXTURE_2D, texture2);
	// set the texture wrapping parameters
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
	// set texture filtering parameters
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	// load image, create texture and generate mipmaps
	width, height, nrChannels;

	unsigned char* data3 = stbi_load("black.jpg", &width, &height, &nrChannels, 0);
	if (data)
	{

		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, data3);
		glGenerateMipmap(GL_TEXTURE_2D);
	}
	else
	{
		std::cout << "Failed to load texture" << std::endl;
	}
	stbi_image_free(data3);

	// texture 3
// ---------
	glGenTextures(1, &texture3);
	glBindTexture(GL_TEXTURE_2D, texture3);
	// set the texture wrapping parameters
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
	// set texture filtering parameters
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	// load image, create texture and generate mipmaps
	width, height, nrChannels;
	stbi_set_flip_vertically_on_load(true); // tell stb_image.h to flip loaded texture's on the y-axis.
	unsigned char* data2 = stbi_load("blackmetal.png", &width, &height, &nrChannels, 0);
	if (data)
	{
		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, data2);
		glGenerateMipmap(GL_TEXTURE_2D);
	}
	else
	{
		std::cout << "Failed to load texture" << std::endl;
	}
	stbi_image_free(data2);

	glGenTextures(1, &texture4);
	glBindTexture(GL_TEXTURE_2D, texture4);
	// set the texture wrapping parameters
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
	// set texture filtering parameters
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	// load image, create texture and generate mipmaps
	width, height, nrChannels;
	unsigned char* data4 = stbi_load("wood.jpg", &width, &height, &nrChannels, 0);
	if (data)
	{
		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, data4);
		glGenerateMipmap(GL_TEXTURE_2D);
	}
	else
	{
		std::cout << "Failed to load texture" << std::endl;
	}
	stbi_image_free(data4);
	
	glGenTextures(1, &texture5);
	glBindTexture(GL_TEXTURE_2D, texture5);
	// set the texture wrapping parameters
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
	// set texture filtering parameters
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	// load image, create texture and generate mipmaps
	width, height, nrChannels;
	stbi_set_flip_vertically_on_load(true); // tell stb_image.h to flip loaded texture's on the y-axis.
	unsigned char* data5 = stbi_load("glass.jpg", &width, &height, &nrChannels, 0);
	if (data)
	{
		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, data5);
		glGenerateMipmap(GL_TEXTURE_2D);
	}
	else
	{
		std::cout << "Failed to load texture" << std::endl;
	}
	stbi_image_free(data5);
	// tell opengl for each sampler to which texture unit it belongs to (only has to be done once)
	// -------------------------------------------------------------------------------------------
	ourShader.use();
	ourShader.setInt("texture1", 0);
	ourShader.setInt("texture2", 1);
	ourShader.setInt("texture3", 2);
	ourShader.setInt("texture4", 3);
	ourShader.setInt("texture5", 4);

	glm::mat4 model;

	float angle;

	// render loop
	// -----------
	while (!glfwWindowShouldClose(window))
	{
		// per-frame time logic
		// --------------------
		float currentFrame = glfwGetTime();
		deltaTime = currentFrame - lastFrame;
		lastFrame = currentFrame;

		// input
		// -----
		processInput(window);

		// render
		// ------
		glClearColor(0.2f, 0.3f, 0.3f, 1.0f);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		// bind textures on corresponding texture units

		//glActiveTexture(GL_TEXTURE1);
		//glBindTexture(GL_TEXTURE_2D, texture2);

		// activate shader
		ourShader.use();

		// If statements to choose perspective or ortho view which is then passed to shader
		if (!changeView) {
			glm::mat4 projection = glm::perspective(glm::radians(fov), (float)SCR_WIDTH / (float)SCR_HEIGHT, 0.1f, 100.0f);
			ourShader.setMat4("projection", projection);
		}
		if (changeView) {
			glm::mat4 projection = glm::ortho(-5.0f, 5.0f, -5.0f, 5.0f, 0.1f, 100.0f);
			ourShader.setMat4("projection", projection);
		}
		

		// camera/view transformation
		glm::mat4 view = glm::lookAt(cameraPos, cameraPos + cameraFront, cameraUp);
		ourShader.setMat4("view", view);

		// setting texture for plane
		glBindTexture(GL_TEXTURE_2D, texture4);
		glBindVertexArray(planeVAO);
		//render plane
		model = model = glm::mat4(1.0f);
		model = glm::translate(model, glm::vec3(2.5f, -0.7f, 0.0f));
		ourShader.setMat4("model", model);

		// draw plane
		glDrawElements(GL_TRIANGLES, planeIndicies, GL_UNSIGNED_SHORT, (void*)planeIndexOffset);

		//texture for the big square
		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_2D, texture1);
		// render big square
		glBindVertexArray(square1VAO);
		// calculate the model matrix 
		model = glm::mat4(1.0f);
		model = glm::translate(model, glm::vec3(0.0f, 0.0f, 0.0f));
		angle = 0.0f;
		model = glm::rotate(model, glm::radians(angle), glm::vec3(1.0f, 0.3f, 0.5f));
		ourShader.setMat4("model", model);
		//render big square
		glDrawArrays(GL_TRIANGLES, 0, 36);

		// texture for small square

		glBindTexture(GL_TEXTURE_2D, texture2);
		//render small square
		glBindVertexArray(square2VAO);
		// calculate the model matrix
		model = glm::mat4(1.0f);
		model = glm::translate(model, glm::vec3(0.0f, 0.0f, 0.1f));
		angle = 0.0f;
		model = glm::rotate(model, glm::radians(angle), glm::vec3(1.0f, 0.3f, 0.5f));
		ourShader.setMat4("model", model);

		glDrawArrays(GL_TRIANGLES, 0, 36);

		// texture for sphere
		glBindTexture(GL_TEXTURE_2D, texture5);
		glBindVertexArray(sphereVAO);
		//model matrix for sphere
		model = model = glm::mat4(1.0f);
		model = glm::translate(model, glm::vec3(0.0f, 1.0f, 0.0f));
		model = glm::scale(model, glm::vec3(0.18f)); // adjusting size of sphere
		ourShader.setMat4("model", model);

		// draw sphere
		glDrawElements(GL_TRIANGLES, sphereIndices, GL_UNSIGNED_SHORT, (void*)sphereIndex);


		//texture for cylinder
		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_2D, texture3);
		//model matrix for cylinder
		glBindVertexArray(cylVAO);
		model = glm::mat4(1.0f);
		model = glm::translate(model, glm::vec3(0.0f, 1.0f, 0.0f));
		ourShader.setMat4("model", model);
		static_meshes_3D::Cylinder C(0.15, 10, 0.6, true, true, true);
		//render cylinder
		C.render();



		// render boxes
		// glfw: swap buffers and poll IO events (keys pressed/released, mouse moved etc.)
		// -------------------------------------------------------------------------------
		glfwSwapBuffers(window);
		glfwPollEvents();
	}


	// ------------------------------------------------------------------------
	glDeleteVertexArrays(1, &square1VAO);
	glDeleteBuffers(1, &square1VBO);

	glDeleteVertexArrays(1, &cylVAO);
	glDeleteBuffers(1, &cylVBO);

	glDeleteVertexArrays(1, &square2VAO);
	glDeleteBuffers(1, &square2VBO);

	glDeleteVertexArrays(1, &sphereVAO);
	glDeleteBuffers(1, &sphereVBO);

	glDeleteVertexArrays(1, &planeVAO);
	glDeleteBuffers(1, &planeVBO);
	// glfw: terminate, clearing all previously allocated GLFW resources.
	// ------------------------------------------------------------------
	glfwTerminate();
	return 0;
}

// process all input: query GLFW whether relevant keys are pressed/released this frame and react accordingly
// ---------------------------------------------------------------------------------------------------------
void processInput(GLFWwindow* window)
{
	if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
		glfwSetWindowShouldClose(window, true);

	float cameraSpeed = cameraSpeedChange * deltaTime;
	if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS)
		cameraPos += cameraSpeed * cameraFront;
	if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS)
		cameraPos -= cameraSpeed * cameraFront;
	if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS)
		cameraPos -= glm::normalize(glm::cross(cameraFront, cameraUp)) * cameraSpeed;
	if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS)
		cameraPos += glm::normalize(glm::cross(cameraFront, cameraUp)) * cameraSpeed;
	if (glfwGetKey(window, GLFW_KEY_Q) == GLFW_PRESS)
		cameraPos += cameraSpeed * cameraUp;
	if (glfwGetKey(window, GLFW_KEY_E) == GLFW_PRESS)
		cameraPos -= cameraSpeed * cameraUp;
	if (glfwGetKey(window, GLFW_KEY_P) == GLFW_PRESS)
		changeView = !changeView;
}

// glfw: whenever the window size changed (by OS or user resize) this callback function executes
// ---------------------------------------------------------------------------------------------
void framebuffer_size_callback(GLFWwindow* window, int width, int height)
{
	// make sure the viewport matches the new window dimensions; note that width and 
	// height will be significantly larger than specified on retina displays.
	glViewport(0, 0, width, height);
}

// glfw: whenever the mouse moves, this callback is called
// -------------------------------------------------------
void mouse_callback(GLFWwindow* window, double xpos, double ypos)
{
	if (firstMouse)
	{
		lastX = xpos;
		lastY = ypos;
		firstMouse = false;
	}

	float xoffset = xpos - lastX;
	float yoffset = lastY - ypos; // reversed since y-coordinates go from bottom to top
	lastX = xpos;
	lastY = ypos;

	float sensitivity = 0.1f; // change this value to your liking
	xoffset *= sensitivity;
	yoffset *= sensitivity;

	yaw += xoffset;
	pitch += yoffset;

	// make sure that when pitch is out of bounds, screen doesn't get flipped
	if (pitch > 89.0f)
		pitch = 89.0f;
	if (pitch < -89.0f)
		pitch = -89.0f;

	glm::vec3 front;
	front.x = cos(glm::radians(yaw)) * cos(glm::radians(pitch));
	front.y = sin(glm::radians(pitch));
	front.z = sin(glm::radians(yaw)) * cos(glm::radians(pitch));
	cameraFront = glm::normalize(front);
}

// glfw: whenever the mouse scroll wheel scrolls, this callback is called
// ----------------------------------------------------------------------
void scroll_callback(GLFWwindow* window, double xoffset, double yoffset)
{
	cameraSpeedChange += (float)yoffset;
	if (cameraSpeedChange < 1.0) {
		cameraSpeedChange = 1.0;
	}
}