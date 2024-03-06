#include <seden.h>
#include <vector>
#include <ft2build.h>
#include FT_FREETYPE_H

#include "objects/path.h"


void processInput(Seden::PerspectiveCamera& camera, GLFWwindow* window);

int main()
{
	Seden::win::init(1280, 720, "vector graphics");
	Seden::win::initGui();
	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	glEnable(GL_PROGRAM_POINT_SIZE);

	float aspect = 1280.f / 720.f;

	Shader basicSh("assets/shaders/pathVert.glsl", "assets/shaders/pathFrag.glsl");

	Seden::PerspectiveCamera cam(aspect);

	std::vector<glm::vec4> varray {{-1,1,0,1}, {-0.5,0,0,1} , {0.5,0,0,1}  , {0.5,0,1,1}, {0.5,1,1,1}, {1,1,1,1}};

	Path path(varray, 10);

	QuadraticBezier bezier({ glm::vec4(0,0,0,1),glm::vec4(0.5,0,0,1),glm::vec4(1,1,0,1) });

	GLuint vao;
	glGenVertexArrays(1, &vao);
	glBindVertexArray(vao);

	float t = 0.0;
	
	//glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
	
	float begin=0;
	float end = 1;
	while (Seden::win::isRunning())
	{
		Seden::win::clear();
		Seden::win::clearGui();
		
		ImGui::Begin("test");
		ImGui::Text(std::to_string(glfwGetTime()).c_str());
		if (ImGui::Button("press")) {
			
			
		}
		else
		{
			
		}
		
		processInput(cam, Seden::win::getWindowRef());

		ImGui::SliderFloat("begin", &begin, 0.f, 1.f);
		ImGui::SliderFloat("end", &end, 0.f, 1.f);
		ImGui::End();

		t += .0001;
		

		basicSh.Bind();
		basicSh.setMat4("u_mvp", cam.getProjectionMatrix()*cam.getViewMatrix());
		basicSh.setVec2("u_resolution", {1280* glm::length(cam.getPosition()), 720 * glm::length(cam.getPosition())});
		basicSh.setFloat("u_thickness", 10);


		path.trimBegin(begin);
		path.trimEnd(end);
		//
		bezier.trimBegin(begin);
		bezier.trimEnd(end);
		
		//bezier.draw();


		path.draw();

		
		
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
	const float cameraSpeed = 5.f * Seden::win::getDeltaTime();
	const float sensitivity = 2.f * Seden::win::getDeltaTime();

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