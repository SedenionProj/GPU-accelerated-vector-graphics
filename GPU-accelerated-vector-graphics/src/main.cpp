#include <seden.h>
#include <vector>
#include <ft2build.h>
#include FT_FREETYPE_H

#include "objects/path.h"


void processInput(Seden::PerspectiveCamera& camera, GLFWwindow* window);

std::vector<Line> tree;

float angleA = glm::pi<float>() / 4;
float angleB = glm::pi<float>() / 4;
float angleC = glm::pi<float>() / 4;
float angleD = glm::pi<float>() / 4;

float scale = 2;

void constructTree(glm::vec4 pos, float alpha, float beta, float len, int level) {
	if (level == 0) {
		return;
	}

	glm::vec4 pos2 = pos + len*glm::vec4(cos(alpha) * sin(beta), sin(alpha) * sin(beta), cos(beta), 0);

	Line(vertex({ {pos }, {1,0,0,1}}),
					    vertex({ {pos2}, {0,1,0,1}})).draw();

	constructTree(pos2, alpha - angleA, beta, len/scale, level-1);
	constructTree(pos2, alpha + angleB, beta, len/scale, level-1);
	constructTree(pos2, alpha, beta + angleC, len/scale, level-1);
	constructTree(pos2, alpha, beta - angleD, len/scale, level-1);
}

int main()
{
	Seden::win::init(1280, 720, "vector graphics");
	Seden::win::initGui();
	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	glEnable(GL_PROGRAM_POINT_SIZE);
	glEnable(GL_DEPTH_TEST);

	float aspect = 1280.f / 720.f;

	Shader basicSh("assets/shaders/pathVert.glsl", "assets/shaders/pathFrag.glsl");

	Seden::PerspectiveCamera cam(aspect);

	//std::vector<glm::vec4> varray{
	//	{-1,1,0,1},
	//	{-0.5,0,0,1} ,
	//	{0.5,0,0,1}  ,
	//	{0.5,0,1,1},
	//	{0.5,1,1,1},
	//	{1,1,1,1} };

	std::vector<vertex> varray {
		{{-1,0,0,1}, {1,0,0,1}  },
		{{-0.5,0,0,1} ,  {1,0,0,1}},
		{{0.5,0,0,1}  ,  {1,0,0,1}},
		{{0.5,0,1,1},  {0,0,1,1}},
		{{0.5,1,1,1},  {1,0,0,1}},
		{{1.5,2,1,1},   {1,0,0,1}      }
	};

	//Path path(varray, 10);

	GLuint vao;
	glGenVertexArrays(1, &vao);
	glBindVertexArray(vao);

	float t = 0.0;
	
	
	constructTree(glm::vec4(0,0,0,1), glm::pi<float>() / 2, glm::pi<float>() / 2, 1, 4);
	
	std::vector<vertex> l2p({
		{ {-1  ,0,-1,1}, {1,0,0,1} },
		{ {-0.5,0,-1,1}, {1,0,0,1} },
		{ { 0.5,1,-2,1}, {1,0,0,1} },
		{ { 1  ,1,-2,1}, {1,0,0,1} } });

	//Path l2(l2p,10);


	float begin = 0;
	float end = 1;
	float size = 5;

	SEDEN_INFO("pause");
	while (Seden::win::isRunning())
	{
		processInput(cam, Seden::win::getWindowRef());
		Seden::win::clear();
		Seden::win::clearGui();
		ImGui::Begin("test");
		ImGui::Text(std::to_string(glfwGetTime()).c_str());
		//ImGui::Text(std::to_string(tree.size()).c_str());
		//ImGui::Text(std::to_string(tree.front().ssbo).c_str());
		if (ImGui::Button("press")) {
			glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
		}
		else
		{
			
		}
		ImGui::SliderFloat("begin", &begin, 0.f, 1.f);
		ImGui::SliderFloat("end", &end, 0.f, 1.f);
		ImGui::SliderFloat("size", &size, 0.f, 100.f);
		ImGui::SliderFloat("angleA", &angleA, 0.f, 7.f);
		ImGui::SliderFloat("angleB", &angleB, 0.f, 7.f);
		ImGui::SliderFloat("angleC", &angleC, 0.f, 7.f);
		ImGui::SliderFloat("angleD", &angleD, 0.f, 7.f);
		ImGui::SliderFloat("scale", &scale, 0.f, 10.f);
		ImGui::End();


		//tree.clear();
		constructTree(glm::vec4(0, 0, 0, 1), glm::pi<float>() / 2, glm::pi<float>() / 2, 1, 7);

		t += .0001;
		

		basicSh.Bind();
		basicSh.setMat4("u_mvp", cam.getProjectionMatrix()*cam.getViewMatrix());
		basicSh.setFloat("u_thickness", size);

		//for (int i = 0; i < tree.size(); i++) {
		//	tree[i].trimBegin(begin);
		//	tree[i].trimEnd(end);
		//	tree[i].draw();
		//}

		
		//l2.trimBegin(begin);
		//l2.trimEnd(end);
		//l2.draw();

		
		//path.trimBegin(begin);
		//path.trimEnd(end);
		//path.draw();

		
		
		Seden::win::drawGui();
		Seden::win::display();
	}

	Seden::win::terminate();
	Seden::win::closeGui();
	//FT_Done_Face(face);
	//FT_Done_FreeType(library);
}

void processInput(Seden::PerspectiveCamera& camera, GLFWwindow* window)
{
	const float cameraSpeed = 2.f * Seden::win::getDeltaTime();
	const float sensitivity = 1.f * Seden::win::getDeltaTime();

	if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS)
		camera.moveFront(cameraSpeed);
	if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS)
		camera.moveFront(-cameraSpeed);
	if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS)
		camera.moveRight(-cameraSpeed);
	if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS)
		camera.moveRight(cameraSpeed);
	if (glfwGetKey(window, GLFW_KEY_SPACE) == GLFW_PRESS)
		camera.moveUp(cameraSpeed);
	if (glfwGetKey(window, GLFW_KEY_LEFT_SHIFT) == GLFW_PRESS)
		camera.moveUp(-cameraSpeed);

	if (glfwGetKey(window, GLFW_KEY_LEFT) == GLFW_PRESS)
		camera.rotate(0.0, -sensitivity);
	if (glfwGetKey(window, GLFW_KEY_RIGHT) == GLFW_PRESS)
		camera.rotate(0.0, sensitivity);
	if (glfwGetKey(window, GLFW_KEY_UP) == GLFW_PRESS)
		camera.rotate(sensitivity, 0.0);
	if (glfwGetKey(window, GLFW_KEY_DOWN) == GLFW_PRESS)
		camera.rotate(-sensitivity, 0.0);
}