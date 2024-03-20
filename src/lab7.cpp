#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include <iostream>

#include "camera.h"
#include "shader.h"

void framebuffer_size_callback(GLFWwindow* window, int width, int height);
void mouse_callback(GLFWwindow* window, double xpos, double ypos);
void scroll_callback(GLFWwindow* window, double xoffset, double yoffset);
void ProcessInput(GLFWwindow* window);
void drawFloorAndCubes(Shader& shader);

// settings
const unsigned int SCR_WIDTH = 1600;
const unsigned int SCR_HEIGHT = 1200;
unsigned int VAO, VBO;

// camera
Camera camera(glm::vec3(0.0f, 0.0f, 3.0f));
float lastX = (float)SCR_WIDTH / 2.0;
float lastY = (float)SCR_HEIGHT / 2.0;
bool firstMouse = true;

// timing
float deltaTime = 0.0f;
float lastFrame = 0.0f;

// light attributes
glm::vec3 lightDirection(+0.1f, -0.81f, -0.61f);
glm::vec3 lightColor(0.0f, 0.0f, 1.0f);
glm::vec3 lightPos(2.0f, 6.0f, 7.0f);

float vertices[] =
{
	//back face
	//pos					//col		     //normal
	-0.5f, -0.5f, -0.5f,  	1.f, 1.0f, 1.0f, 0, 0, -1,
	0.5f, -0.5f, -0.5f,  	1.f, 1.0f, 1.0f, 0, 0, -1,
	0.5f,  0.5f, -0.5f,  	1.f, 1.0f, 1.0f, 0, 0, -1,
	0.5f,  0.5f, -0.5f,  	1.f, 1.0f, 1.0f, 0, 0, -1,
	-0.5f,  0.5f, -0.5f,  	1.f, 1.0f, 1.0f, 0, 0, -1,
	-0.5f, -0.5f, -0.5f,  	1.f, 1.0f, 1.0f, 0, 0, -1,

	//front face
	-0.5f, -0.5f,  0.5f,  	1.0f, 1.0f, 1.0f, 0, 0, 1,
	0.5f, -0.5f,  0.5f,  	1.0f, 1.0f, 1.0f, 0, 0, 1,
	0.5f,  0.5f,  0.5f,  	1.0f, 1.0f, 1.0f, 0, 0, 1,
	0.5f,  0.5f,  0.5f,  	1.0f, 1.0f, 1.0f, 0, 0, 1,
	-0.5f,  0.5f,  0.5f,  	1.0f, 1.0f, 1.0f, 0, 0, 1,
	-0.5f, -0.5f,  0.5f,  	1.0f, 1.0f, 1.0f, 0, 0, 1,

	//left face
	-0.5f,  0.5f,  0.5f,  	1.0f, 1.0f, 1.0f, -1, 0, 0,
	-0.5f,  0.5f, -0.5f,  	1.0f, 1.0f, 1.0f, -1, 0, 0,
	-0.5f, -0.5f, -0.5f,  	1.0f, 1.0f, 1.0f, -1, 0, 0,
	-0.5f, -0.5f, -0.5f,  	1.0f, 1.0f, 1.0f, -1, 0, 0,
	-0.5f, -0.5f,  0.5f,  	1.0f, 1.0f, 1.0f, -1, 0, 0,
	-0.5f,  0.5f,  0.5f,  	1.0f, 1.0f, 1.0f, -1, 0, 0,

	//right face
	0.5f,  0.5f,  0.5f,  	1.f, 1.0f, 1.0f, 1, 0, 0,
	0.5f,  0.5f, -0.5f,  	1.f, 1.0f, 1.0f, 1, 0, 0,
	0.5f, -0.5f, -0.5f, 	1.f, 1.0f, 1.0f, 1, 0, 0,
	0.5f, -0.5f, -0.5f,  	1.f, 1.0f, 1.0f, 1, 0, 0,
	0.5f, -0.5f,  0.5f,  	1.f, 1.0f, 1.0f, 1, 0, 0,
	0.5f,  0.5f,  0.5f,  	1.f, 1.0f, 1.0f, 1, 0, 0,

	//bottom face
	-0.5f, -0.5f, -0.5f,  	1.f, 1.0f, 1.0f, 0, -1, 0, 
	0.5f, -0.5f, -0.5f,  	1.f, 1.0f, 1.0f, 0, -1, 0,
	0.5f, -0.5f,  0.5f,  	1.f, 1.0f, 1.0f, 0, -1, 0,
	0.5f, -0.5f,  0.5f,  	1.f, 1.0f, 1.0f, 0, -1, 0,
	-0.5f, -0.5f,  0.5f,  	1.f, 1.0f, 1.0f, 0, -1, 0,
	-0.5f, -0.5f, -0.5f,  	1.f, 1.0f, 1.0f, 0, -1, 0,

	//top face
	-0.5f,  0.5f, -0.5f,  	1.0f, 1.f, 1.0f, 0, 1, 0,
	0.5f,  0.5f, -0.5f,  	1.0f, 1.f, 1.0f, 0, 1, 0,
	0.5f,  0.5f,  0.5f,  	1.0f, 1.f, 1.0f, 0, 1, 0,
	0.5f,  0.5f,  0.5f,  	1.0f, 1.f, 1.0f, 0, 1, 0,
	-0.5f,  0.5f,  0.5f,  	1.0f, 1.f, 1.0f, 0, 1, 0,
	-0.5f,  0.5f, -0.5f, 	1.0f, 1.f, 1.0f, 0, 1, 0
};

int main()
{
	glfwInit();

	GLFWwindow* window = glfwCreateWindow(SCR_WIDTH, SCR_HEIGHT, "lab7", nullptr, nullptr);
	glfwMakeContextCurrent(window);
	glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);
	glfwSetCursorPosCallback(window, mouse_callback);
	glfwSetScrollCallback(window, scroll_callback);
	glewInit();
	glEnable(GL_DEPTH_TEST);

	Shader shader("res/shaders/lab7.vs", "res/shaders/lab7.fs");

	// config VAO, VBO
	glGenVertexArrays(1, &VAO);
	glGenBuffers(1, &VBO);

	glBindVertexArray(VAO);
	glBindBuffer(GL_ARRAY_BUFFER, VBO);
	glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 9 * sizeof(float), (void*)0);
	glEnableVertexAttribArray(0);
	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 9 * sizeof(float), (void*)(3 * sizeof(float)));
	glEnableVertexAttribArray(1);
	glVertexAttribPointer(2, 3, GL_FLOAT, GL_FALSE, 9 * sizeof(float), (void*)(6 * sizeof(float)));
	glEnableVertexAttribArray(2);

	while (!glfwWindowShouldClose(window)) {
		// Per-frame logic
		float currentFrame = glfwGetTime();
		deltaTime = currentFrame - lastFrame;
		lastFrame = currentFrame;

		// clear buffer
		glClearColor(0.1f, 0.1f, 0.1f, 1.0f);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		// process input
		ProcessInput(window);

		glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);

		// bind shader
		shader.Bind();

		glm::mat4 view = camera.GetViewMatrix();
		glm::mat4 projection = glm::perspective(camera.fov, (float)SCR_WIDTH / (float)SCR_HEIGHT, 0.1f, 100.f);
		glm::mat4 model = glm::mat4(1.0f);

		shader.SetMat4("projection", projection);
		shader.SetMat4("view", view);

		shader.SetVec3("lightDirection", lightDirection);
		shader.SetVec3("lightColor", lightColor);
		shader.SetVec3("lightPos", lightPos);
		shader.SetVec3("viewPos", camera.position);
		//shader.SetMat4("model", model);

		// rendering
		drawFloorAndCubes(shader);

		// ---------------------
		glfwSwapBuffers(window);
		glfwPollEvents();
	}

	glfwTerminate();
}

void drawFloorAndCubes(Shader& shader)
{
	glBindVertexArray(VAO);

	//floor
	shader.Bind();
	glm::mat4 model = glm::mat4(1.f);
	model = glm::translate(model, glm::vec3(0, -3, 0));
	model = glm::scale(model, glm::vec3(100, 0.1, 100));
	shader.SetMat4("model", model);
	glDrawArrays(GL_TRIANGLES, 0, 36);

	//cubes
	for (int i = -1; i < 2; i++) {
		for (int j = -1; j < 2; j++) {
			for (int k = -1; k < 2; k++) {
				glm::mat4 model = glm::mat4(1.f);
				model = glm::translate(model, glm::vec3(float(i * 2), float(j * 2), float(k * 2)));
				shader.SetMat4("model", model);
				glDrawArrays(GL_TRIANGLES, 0, 36);
			}
		}
	}
	glBindVertexArray(0);
}

// process all input: query GLFW whether relevant keys are pressed/released this frame and react accordingly
// ---------------------------------------------------------------------------------------------------------
void ProcessInput(GLFWwindow* window)
{
	if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
		glfwSetWindowShouldClose(window, true);

	if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS)
		camera.ProcessKeyboard(FORWARD, deltaTime);
	if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS)
		camera.ProcessKeyboard(BACKWARD, deltaTime);
	if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS)
		camera.ProcessKeyboard(LEFT, deltaTime);
	if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS)
		camera.ProcessKeyboard(RIGHT, deltaTime);

	if (glfwGetKey(window, GLFW_KEY_SPACE) == GLFW_PRESS) {
		lightPos = camera.position;
	}
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
void mouse_callback(GLFWwindow* window, double xposIn, double yposIn)
{
	float xpos = static_cast<float>(xposIn);
	float ypos = static_cast<float>(yposIn);
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

	camera.ProcessMouseMovement(xoffset, yoffset);
}

// glfw: whenever the mouse scroll wheel scrolls, this callback is called
// ----------------------------------------------------------------------
void scroll_callback(GLFWwindow* window, double xoffset, double yoffset)
{
	camera.ProcessMouseScroll(static_cast<float>(yoffset));
}