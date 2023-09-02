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
const unsigned int SCR_WIDTH = 1600;
const unsigned int SCR_HEIGHT = 1200;

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

	// Load model(s)
	Model rock("res/models/rock/rock.obj");
	Model mars("res/models/planet/planet.obj");

	// Build & compile shader(s)
	Shader marsShader("res/shaders/instancing.vs", "res/shaders/instancing_mars.fs");
	Shader rockShader("res/shaders/instancing.vs", "res/shaders/instancing_rock.fs");

	// Generate a large list of semi-random model transformation matrices
	unsigned int amount = 1000;
	glm::mat4* modelMatrices = new glm::mat4[amount];
	glm::vec3* axis = new glm::vec3[amount]; // Stores the rotation axis for each rock instance

	std::random_device rd;
	std::mt19937 gen(rd()); // Initialize Mersenne Twister random number generator
	std::uniform_real_distribution<float> dis(-1.0, 1.0);
	std::uniform_real_distribution<float> scaleDis(0.05, 0.25);
	std::uniform_real_distribution<float> angleDis(0.0, 360.0);
	std::uniform_real_distribution<float> axisDis(0.0, 1.0);

	float radius = 50.0f;

	// Offset: Controls the random displacement of each rock. 
    // Choose a rational range to minimize rock collisions.
	float offset = 3.0f; 

	// Loop to initialize each rock's model matrix
	for (size_t i = 0; i < amount; i++) {
		glm::mat4 model = glm::mat4(1.0f);

		// 1. Translation: Displace each rock along a circle with a radius and a random offset
		float angle = (float)(i) / (float)(amount) * 360.0f;
		float x = sin(angle) * radius + dis(gen) * offset;
		float y = 0.4 * dis(gen) * offset;
		float z = cos(angle) * radius + dis(gen) * offset;
		model = glm::translate(model, glm::vec3(x, y, z));

		// 2. Scaling: Randomly scale each rock
		float scale = scaleDis(gen);
		model = glm::scale(model, glm::vec3(scale));

		// 3. Rotation: Apply a random initial rotation around a random axis
		float rotAngle = angleDis(gen);
		glm::vec3 randomAxis(axisDis(gen), axisDis(gen), axisDis(gen));
		model = glm::rotate(model, glm::radians(rotAngle), randomAxis);
		axis[i] = randomAxis; // Store this axis for later use

		// 4. Store the model matrix
		modelMatrices[i] = model;
	}

	// Generate a random rotation speed array for rocks
	float* rotationSpeeds = new float[amount];
	for (size_t i = 0; i < amount; i++) {
		std::uniform_real_distribution<float>angleDistribution(4.0f, 8.0f);
		rotationSpeeds[i] = angleDistribution(gen);  // Random rotation speed
	}

	int counter = 0;
	const int maxPrints = 30;
	while (!glfwWindowShouldClose(window)) {
		// Per-frame logic
		float currentFrame = glfwGetTime();
		deltaTime = currentFrame - lastFrame;
		lastFrame = currentFrame;
		if (counter < maxPrints) {
			std::cout << "Delta Time: " << deltaTime << "\n";
			std::cout << "fps: " << 1.0f / deltaTime << "\n";
			counter++;
		}
		
		// Render
		glClearColor(0.1f, 0.1f, 0.1f, 1.0f);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		// Process input
		ProcessInput(window);

		// Update the rotation of each rock around its own random axis at a random speed.
		// Using deltaTime to ensure frame-rate independent rotation
		for (unsigned int i = 0; i < amount; i++) {
			glm::mat4 model = modelMatrices[i];
			float angle = rotationSpeeds[i] * deltaTime;
			model = glm::rotate(model, angle, axis[i]); // random angle & random axis
			modelMatrices[i] = model;
		}

		// Configure transformation matrices
		glm::mat4 projection = glm::perspective(glm::radians(45.0f), (float)SCR_WIDTH / (float)SCR_HEIGHT, 0.1f, 1000.0f);
		glm::mat4 view = camera.GetViewMatrix();
		glm::mat4 model = glm::mat4(1.0f);

		// Draw planet(mars)
		marsShader.Bind();
		marsShader.SetMat4("projection", projection);
		marsShader.SetMat4("view", view);
		model = glm::translate(model, glm::vec3(0.0f, -3.0f, 0.0f));
		model = glm::scale(model, glm::vec3(4.0f, 4.0f, 4.0f));
		marsShader.SetMat4("model", model);
		mars.Draw(marsShader);

		// Draw amount of rocks
		rockShader.Bind();
		rockShader.SetMat4("projection", projection);
		rockShader.SetMat4("view", view);
		for (size_t i = 0; i < amount; i++) {
			rockShader.SetMat4("model", modelMatrices[i]);
			rock.Draw(rockShader);
		}

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