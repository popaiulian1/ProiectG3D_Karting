#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include "Camera.h"
#include "Shader.h"
#include <filesystem>
#include "Model.h"

Camera* pCamera = nullptr;

void Cleanup()
{
	delete pCamera;
}

float deltaTime = 0.0f;
float lastFrame = 0.0f;

void framebuffer_size_callback(GLFWwindow* window, int width, int height);
void mouse_callback(GLFWwindow* window, double xpos, double ypos);
void scroll_callback(GLFWwindow* window, double xoffset, double yoffset);
void processInput(GLFWwindow* window);
unsigned int CreateTexture(const std::string& strTexturePath);

int main(int argc, char** argv) {

	std::string strFullExeFileName = argv[0];
	std::string strExePath;
	const size_t last_slash_idx = strFullExeFileName.rfind('\\');
	if (std::string::npos != last_slash_idx)
	{
		strExePath = strFullExeFileName.substr(0, last_slash_idx);
	}

	glfwInit();
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

	GLFWwindow* window = glfwCreateWindow(800, 600, "KartingG3d", NULL, NULL);
	if (window == NULL)
	{
		std::cout << "Failed to create GLFW window" << std::endl;
		glfwTerminate();
		Cleanup();
		return -1;
	}

	glfwMakeContextCurrent(window);
	glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);
	glfwSetCursorPosCallback(window, mouse_callback);
	glfwSetScrollCallback(window, scroll_callback);
	glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);

	glewInit();

	glEnable(GL_DEPTH_TEST);
	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

	float floorVertices[] = {
		// positions          // texture Coords 
		 20.0f, -0.5f,  20.0f,  1.0f, 0.0f,
		-20.0f, -0.5f,  20.0f,  0.0f, 0.0f,
		-20.0f, -0.5f, -20.0f,  0.0f, 1.0f,

		 20.0f, -0.5f,  20.0f,  1.0f, 0.0f,
		-20.0f, -0.5f, -20.0f,  0.0f, 1.0f,
		 20.0f, -0.5f, -20.0f,  1.0f, 1.0f
	};

	unsigned floorVAO, floorVBO;
	glGenVertexArrays(1, &floorVAO);
	glGenBuffers(1, &floorVBO);
	glBindVertexArray(floorVAO);
	glBindBuffer(GL_ARRAY_BUFFER, floorVBO);
	glBufferData(GL_ARRAY_BUFFER, sizeof(floorVertices), &floorVertices, GL_STATIC_DRAW);
	glEnableVertexAttribArray(0);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)0);
	glEnableVertexAttribArray(1);
	glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)(3 * sizeof(float)));

	pCamera = new Camera(800, 600, glm::vec3(-30.0f, 0.0f, 3.0f));

	Shader floorShader("floor.vs", "floor.fs");
	Shader kartingShader("karting.vs", "karting.fs");

	std::string kartingModelPath = "..\\thirdparty\\RESOURCES\\gokart\\gokart.obj";
	Model kartingModelObj(kartingModelPath, false);

	while (!glfwWindowShouldClose(window)) {
		double currentFrame = glfwGetTime();
		deltaTime = currentFrame - lastFrame;
		lastFrame = currentFrame;

		processInput(window);

		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		glm::mat4 view = pCamera->GetViewMatrix();
		glm::mat4 projection = pCamera->GetProjectionMatrix();
		
		kartingShader.Use();
		kartingShader.SetVec3("objectColor", 0.5f, 1.0f, 0.31f);
		kartingShader.SetVec3("lightColor", 1.0f, 1.0f, 1.0f);
		kartingShader.SetVec3("lightPos", glm::vec3(0.0f));
		kartingShader.SetVec3("viewPos", pCamera->GetPosition());
		kartingShader.SetInt("texture_diffuse1", 0);
		kartingShader.SetMat4("projection", projection);
		kartingShader.SetMat4("view", pCamera->GetViewMatrix());

		glm::mat4 kartingModel = glm::scale(glm::mat4(1.0f), glm::vec3(0.01f, 0.01f, 0.01f));
		kartingModel = glm::rotate(kartingModel, glm::radians(-90.0f), glm::vec3(1.0f, 0.0f, 0.0f));
		kartingModel = glm::translate(kartingModel, glm::vec3(-30.0f, -0.5f, 3.0f));
		kartingShader.SetMat4("model", kartingModel);
		kartingModelObj.Draw(kartingShader);
		
		glm::mat4 model = glm::mat4(1.0f);
		floorShader.Use();
		floorShader.SetMat4("view", view);
		floorShader.SetMat4("projection", projection);
		floorShader.SetMat4("model", model);

		glBindVertexArray(floorVAO);
		glDrawArrays(GL_TRIANGLES, 0, 6);

		glfwSwapBuffers(window);
		glfwPollEvents();
	}

	Cleanup();
	glDeleteVertexArrays(1, &floorVAO);
	glDeleteBuffers(1, &floorVBO);

	glfwTerminate();
	return 0;
}

void processInput(GLFWwindow* window)
{
	if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
		glfwSetWindowShouldClose(window, true);

	if (glfwGetKey(window, GLFW_KEY_PAGE_UP) == GLFW_PRESS)
		pCamera->ProcessKeyboard(Camera::UP, (float)deltaTime);
	if (glfwGetKey(window, GLFW_KEY_PAGE_DOWN) == GLFW_PRESS)
		pCamera->ProcessKeyboard(Camera::DOWN, (float)deltaTime);
	if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS)
		pCamera->ProcessKeyboard(Camera::LEFT, (float)deltaTime);
	if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS)
		pCamera->ProcessKeyboard(Camera::RIGHT, (float)deltaTime);
	if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS)
		pCamera->ProcessKeyboard(Camera::FORWARD, (float)deltaTime);
	if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS)
		pCamera->ProcessKeyboard(Camera::BACKWARD, (float)deltaTime);

	if (glfwGetKey(window, GLFW_KEY_R) == GLFW_PRESS) {
		int width, height;
		glfwGetWindowSize(window, &width, &height);
		pCamera->Reset(width, height);
	}
}

// glfw: whenever the window size changed (by OS or user resize) this callback function executes
// ---------------------------------------------------------------------------------------------
void framebuffer_size_callback(GLFWwindow* window, int width, int height)
{
	// make sure the viewport matches the new window dimensions; note that width and 
	// height will be significantly larger than specified on retina displays.
	pCamera->Reshape(width, height);
}

void mouse_callback(GLFWwindow* window, double xpos, double ypos)
{
	pCamera->MouseControl((float)xpos, (float)ypos);
}

void scroll_callback(GLFWwindow* window, double xoffset, double yOffset)
{
	pCamera->ProcessMouseScroll((float)yOffset);
}