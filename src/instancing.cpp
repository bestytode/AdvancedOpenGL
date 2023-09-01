#include <random>
#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <glm/gtc/type_ptr.hpp>

#include "camera.h"
#include "model.h"
#include "shader.h"

void framebuffer_size_callback(GLFWwindow* window, int width, int height);
void mouse_callback(GLFWwindow* window, double xpos, double ypos);
void scroll_callback(GLFWwindow* window, double xoffset, double yoffset);
void ProcessInput(GLFWwindow* window);

// settings
const unsigned int SCR_WIDTH = 800;
const unsigned int SCR_HEIGHT = 600;

// camera
Camera camera(glm::vec3(35.0f, 0.0f, 35.0f));
float lastX = (float)SCR_WIDTH / 2.0;
float lastY = (float)SCR_HEIGHT / 2.0;
bool firstMouse = true;

// timing
float deltaTime = 0.0f;
float lastFrame = 0.0f;

int main()
{
	// glfw & glew configs
	GLFWwindow* window = nullptr;
	try {
		if (!glfwInit())
			throw std::runtime_error("failed to init glfw");
		window = glfwCreateWindow(SCR_WIDTH, SCR_HEIGHT, "hnzz", nullptr, nullptr);
		if (!window)
			throw std::runtime_error("failed to create window");

		glfwMakeContextCurrent(window);
		glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);
		glfwSetCursorPosCallback(window, mouse_callback);
		glfwSetScrollCallback(window, scroll_callback);
		glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);

		if (glewInit() != GLEW_OK)
			throw std::runtime_error("failed to init glew");

		// global opengl configs
		glEnable(GL_DEPTH_TEST);
	}
	catch (const std::runtime_error& e) {
		std::cerr << e.what() << std::endl;
		return -1;
	}

	//// Define translation array
	//glm::vec2 translations[100];
	//int index = 0;
	//float offset = 0.1f;
	//for (int y = -10; y < 10; y += 2) {
	//	for (int x = -10; x < 10; x += 2) {
	//		glm::vec2 translation;
	//		translation.x = (float)x / 10.0f + offset;
	//		translation.y = (float)y / 10.0f + offset;
	//		translations[index++] = translation;
	//	}
	//}

	//// store instance data in an array buffer
	//unsigned int instanceVBO;
	//glGenBuffers(1, &instanceVBO);
	//glBindBuffer(GL_ARRAY_BUFFER, instanceVBO);
	//glBufferData(GL_ARRAY_BUFFER, sizeof(glm::vec2) * 100, &translations[0], GL_STATIC_DRAW);
	//glBindBuffer(GL_ARRAY_BUFFER, 0);

	//// vertices config
	//float quadVertices[] = {
	//	// Positions     // Color 
	//	-0.05f,  0.05f,  1.0f, 0.0f, 0.0f,
	//	 0.05f, -0.05f,  0.0f, 1.0f, 0.0f,
	//	-0.05f, -0.05f,  0.0f, 0.0f, 1.0f,

	//	-0.05f,  0.05f,  1.0f, 0.0f, 0.0f,
	//	 0.05f, -0.05f,  0.0f, 1.0f, 0.0f,
	//	 0.05f,  0.05f,  0.0f, 1.0f, 1.0f
	//};

	//unsigned int VAO, VBO;
	//glGenVertexArrays(1, &VAO);
	//glGenBuffers(1, &VBO);
	//glBindVertexArray(VAO);
	//glBindBuffer(GL_ARRAY_BUFFER, VBO);
	//glBufferData(GL_ARRAY_BUFFER, sizeof(quadVertices), quadVertices, GL_STATIC_DRAW);

	//glEnableVertexAttribArray(0);
	//glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)0);
	//glEnableVertexAttribArray(1);
	//glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)(2 * sizeof(float)));
	//glEnableVertexAttribArray(2);
	//glBindBuffer(GL_ARRAY_BUFFER, instanceVBO); // this attribute is from a different vertex buffer
	//glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 2 * sizeof(float), (void*)0);
	//glBindBuffer(GL_ARRAY_BUFFER, 0);
	//glVertexAttribDivisor(2, 1); // important: this tell OpenGL an instanced vertex attribute.

	// Load model
	Model rock("res/models/rock/rock.obj");
	Model planet("res/models/planet/planet.obj");

	// Build & compile shader(s)
	Shader marsShader("res/shaders/instancing.vs", "res/shaders/instancing_mars.fs");
	Shader rockShader("res/shaders/instancing.vs", "res/shaders/instancing_rock.fs");
	// generate a large list of semi-random model transformation matrices
	unsigned int amount = 1000;
	glm::mat4* modelMatrices = new glm::mat4[amount];

	std::random_device rd;
	std::mt19937 gen(rd()); // Mersenne Twister generator
	std::uniform_real_distribution<float> dis(-1.0, 1.0); 
	std::uniform_real_distribution<float> scaleDis(0.05, 0.25); 
	std::uniform_real_distribution<float> angleDis(0.0, 360.0); 

	float radius = 50.0f;
	float offset = 2.5f;

	for (unsigned int i = 0; i < amount; i++) {
		glm::mat4 model = glm::mat4(1.0f);

		// 1. translation, displace along circle with 'radius' in range [-offset, offset]
		float angle = (float)(i) / (float)(amount) * 360.0f;

		// dis(gen) * offset stands for [-offset, offset]
		float x = sin(angle) * radius + dis(gen) * offset;
		float y = 0.4 * dis(gen) * offset;
		float z = cos(angle) * radius + dis(gen) * offset;
		model = glm::translate(model, glm::vec3(x, y, z));

		// 2. scale (0.05 to 0.25)
		float scale = scaleDis(gen);
		model = glm::scale(model, glm::vec3(scale));

		// 3. rotation (0 to 360)
		float rotAngle = angleDis(gen);
		model = glm::rotate(model, rotAngle, glm::vec3(0.4f, 0.6f, 0.8f));

		// 4. add to list of matrices
		modelMatrices[i] = model;
	}

	while (!glfwWindowShouldClose(window)) {
		float currentFrame = glfwGetTime();
		deltaTime = currentFrame - lastFrame;
		lastFrame = currentFrame;

		// render
		glClearColor(0.1f, 0.1f, 0.1f, 1.0f);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		ProcessInput(window);

		// configure transformation matrices
		glm::mat4 projection = glm::perspective(glm::radians(45.0f), (float)SCR_WIDTH / (float)SCR_HEIGHT, 0.1f, 1000.0f);
		glm::mat4 view = camera.GetViewMatrix();;
		glm::mat4 model = glm::mat4(1.0f);

		// draw planet(mars)
		marsShader.Bind();
		marsShader.SetMat4("projection", projection);
		marsShader.SetMat4("view", view);
		model = glm::translate(model, glm::vec3(0.0f, -3.0f, 0.0f));
		model = glm::scale(model, glm::vec3(4.0f, 4.0f, 4.0f));
		marsShader.SetMat4("model", model);
		planet.Draw(marsShader);
		marsShader.Unbind();

		// draw rocks
		rockShader.Bind();
		rockShader.SetMat4("projection", projection);
		rockShader.SetMat4("view", view);
		for (unsigned int i = 0; i < amount; i++) {
			rockShader.SetMat4("model", modelMatrices[i]);
			rock.Draw(rockShader);
		}
		rockShader.Unbind();

		// glfw: swap buffers and poll IO events (keys pressed/released, mouse moved etc.)
		glfwSwapBuffers(window);
		glfwPollEvents();
	}

	glfwTerminate();
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