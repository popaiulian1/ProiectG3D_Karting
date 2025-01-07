#include <Windows.h>
#include <locale>
#include <codecvt>

#include <stdlib.h>
#include <stdio.h>
#include <math.h> 
#include "Shader.h"

#include <gtc/type_ptr.hpp>
#include <glfw3.h>

#include <iostream>
#include <fstream>
#include <sstream>
#include "Model.h"
#include "Camera.h"

#pragma comment (lib, "glfw3dll.lib")
#pragma comment (lib, "glew32.lib")
#pragma comment (lib, "OpenGL32.lib")

const unsigned int SCR_WIDTH = 800;
const unsigned int SCR_HEIGHT = 600;
bool lockedCamera = false;

//Kart movement vars
float kartRotationAngle = 0.0f; // In degrees
float kartRotationSpeed = 50.0f;
float kartSpeed = 0.0f;
float kartAcceleration = 0.0f;
float kartMaxSpeed = 80.0f;
float kartAccelerationSpeed = 10.0f;
float kartDecelerationSpeed = 20.0f;

GLuint ProjectMatrixLocation, ViewMatrixLocation, WorldMatrixLocation;
glm::vec3 startFinishLineCameraPos(-190.0f, 20.0f, 260.0f);
glm::vec3 startFinishLinePos(-190.0f, 30.f, 260.0f);
glm::vec3 kartPos = startFinishLinePos;
Camera* pCamera = nullptr;

double deltaTime = 0.0f;
double lastFrame = 0.0f;

void Cleanup();
void framebuffer_size_callback(GLFWwindow* window, int width, int height);
void mouse_callback(GLFWwindow* window, double xpos, double ypos);
void scroll_callback(GLFWwindow* window, double xoffset, double yoffset);
void processInput(GLFWwindow* window);

int main()
{
	glfwInit();
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

	GLFWwindow* window = glfwCreateWindow(SCR_WIDTH, SCR_HEIGHT, "KartingG3D", NULL, NULL);
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
	glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);

	if (glewInit() != GLEW_OK) {
		std::cout << "Failed to initialize GLEW" << std::endl;
		return -1;
	}

	glEnable(GL_DEPTH_TEST);

	GLenum err = glGetError();
	if (err != GL_NO_ERROR) {
		std::cout << "OpenGL Error: " << err << std::endl;
	}

	pCamera = new Camera(SCR_WIDTH, SCR_HEIGHT, startFinishLineCameraPos);

	glm::vec3 lightPos(0.0f, 200.0f, 0.0f);

	wchar_t buffer[MAX_PATH];
	GetCurrentDirectoryW(MAX_PATH, buffer);

	std::wstring executablePath(buffer);
	std::wstring wscurrentPath = executablePath.substr(0, executablePath.find_last_of(L"\\/"));

	std::wstring_convert<std::codecvt_utf8<wchar_t>> converter;
	std::string currentPath = converter.to_bytes(wscurrentPath);

	Shader lightShader("light.vs", "light.fs");
	Shader trackShader("track.vs", "track.fs");
	Shader kartShader("kart.vs", "kart.fs");

	Model kartModel((currentPath + "\\models\\Kart\\go_kart.obj").c_str(), false);
	Model trackModel((currentPath + "\\models\\Track\\track.obj").c_str(), false);

	while (!glfwWindowShouldClose(window)) {
		double currentFrame = glfwGetTime();
		deltaTime = currentFrame - lastFrame;
		lastFrame = currentFrame;

		lightPos.x = 2.5 * cos(glfwGetTime());
		lightPos.z = 2.5 * sin(glfwGetTime());

		processInput(window);

		glClearColor(0.2f, 0.3f, 0.3f, 1.0f);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		trackShader.use();
		trackShader.SetVec3("objectColor", 1.0f, 0.5f, 0.31f);
		trackShader.SetVec3("lightColor", 1.0f, 1.0f, 1.0f);
		trackShader.SetVec3("lightPos", lightPos);
		trackShader.SetVec3("viewPos", pCamera->GetPosition());
		trackShader.setInt("texture_diffuse1", 0);
		trackShader.setMat4("projection", pCamera->GetProjectionMatrix());
		trackShader.setMat4("view", pCamera->GetViewMatrix());
		glm::mat4 trackModelMatrix = glm::scale(glm::mat4(100.0f), glm::vec3(0.1f, 0.1f, 0.1f));
		trackModelMatrix = glm::translate(trackModelMatrix, glm::vec3(0.0f, 0.0f, 0.0f));
		trackModelMatrix = glm::rotate(trackModelMatrix, glm::radians(-90.0f), glm::vec3(0.0f, 1.0f, 0.0f));
		trackShader.setMat4("model", trackModelMatrix);
		trackModel.Draw(trackShader);

		lightShader.use();
		lightShader.setMat4("projection", pCamera->GetProjectionMatrix());
		lightShader.setMat4("view", pCamera->GetViewMatrix());
		glm::mat4 lightModel = glm::translate(glm::mat4(1.0f), lightPos);
		lightModel = glm::scale(lightModel, glm::vec3(0.05f));
		lightShader.setMat4("model", lightModel);
		
		kartShader.use();
		kartShader.SetVec3("objectColor", 1.0f, 0.5f, 0.31f);
		kartShader.SetVec3("lightColor", 1.0f, 1.0f, 1.0f);
		kartShader.SetVec3("lightPos", lightPos);
		kartShader.SetVec3("viewPos", pCamera->GetPosition());
		kartShader.setInt("texture_diffuse1", 0);
		kartShader.setMat4("projection", pCamera->GetProjectionMatrix());
		kartShader.setMat4("view", pCamera->GetViewMatrix());
		glm::mat4 kartModelMatrix = glm::scale(glm::mat4(1.0f), glm::vec3(0.1f, 0.1f, 0.1f));

		std::cout << "kartPos: (" << kartPos.x << ", " << kartPos.y << ", " << kartPos.z << ")" << std::endl;

		kartModelMatrix = glm::translate(kartModelMatrix, kartPos);
		kartModelMatrix = glm::rotate(kartModelMatrix, glm::radians(kartRotationAngle), glm::vec3(0.0f, 1.0f, 0.0f));
		kartShader.setMat4("model", kartModelMatrix);
		kartModel.Draw(kartShader);

		glfwSwapBuffers(window);
		glfwPollEvents();
	}

	return 0;
}

void Cleanup() {
	if (pCamera)
	{
		delete pCamera;
		pCamera = nullptr;
	}
}

void framebuffer_size_callback(GLFWwindow* window, int width, int height)
{
	pCamera->Reshape(width, height);
}

void mouse_callback(GLFWwindow* window, double xpos, double ypos)
{
	pCamera->MouseControl((float)xpos, (float)ypos);
}

void scroll_callback(GLFWwindow* window, double xoffset, double yoffset)
{
	pCamera->ProcessMouseScroll((float)yoffset);
}

void processInput(GLFWwindow* window)
{

	if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
		glfwSetWindowShouldClose(window, true);

	if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS)
	{
		if (!lockedCamera) {
			pCamera->ProcessKeyboard(LEFT, (float)deltaTime);
		}
		else {
			 // Rotate left
		}
	}
	if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS) {
		if (!lockedCamera) {
			pCamera->ProcessKeyboard(RIGHT, (float)deltaTime);
		}
		else {

		}
		
	}
	if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS)
	{
		if (!lockedCamera) {
			pCamera->ProcessKeyboard(FORWARD, (float)deltaTime);
		}
		else {
			
		}
	}
	if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS)
	{
		if (!lockedCamera) {
			pCamera->ProcessKeyboard(BACKWARD, (float)deltaTime);
		}
		else {
			
		}
	}
	if (glfwGetKey(window, GLFW_KEY_SPACE) == GLFW_PRESS)
	{
		delete pCamera;
		pCamera = new Camera(SCR_WIDTH, SCR_HEIGHT, glm::vec3(-19.0f, 10.0f, 22.0f));
		pCamera->SetCameraSpeedFactor(0.1f);
		lockedCamera = true;
	}

	if (glfwGetKey(window, GLFW_KEY_R) == GLFW_PRESS) {
		int width, height;
		glfwGetWindowSize(window, &width, &height);
		pCamera->Reset(width, height);
		pCamera->SetCameraSpeedFactor(100.0f);
		lockedCamera = false;
	}
}