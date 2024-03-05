﻿#include <seden.h>
#include <vector>
#include <ft2build.h>
#include FT_FREETYPE_H

#include "objects/path.h"




int main()
{
	Seden::win::init(1280, 720, "vector graphics");
	Seden::win::initGui();
	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	glEnable(GL_PROGRAM_POINT_SIZE);

	float aspect = 1280.f / 720.f;

	Shader basicSh("assets/shaders/pathVert.glsl", "assets/shaders/pathFrag.glsl");
	auto projection = glm::ortho(-aspect, aspect, -1.0f, 1.0f, -10.0f, 10.0f);

	std::vector<glm::vec4> varray = { {-1,0,0,1}, {-0.9,0,0,1}, {0.5,0,0,1}, {0.5, 0.1, 0, 1}, {1, 0.1, 0, 1}, {1.1, 0.5, 0, 1}, {1.5, 0.7, 0, 1}, {1.7, 0.0, 0, 1} };
	Path path(varray, 10);

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
		
		ImGui::SliderFloat("begin", &begin, 0.f, 1.f);
		ImGui::SliderFloat("end", &end, 0.f, 1.f);
		ImGui::End();

		t += .001;
		

		basicSh.Bind();
		basicSh.setMat4("u_mvp", projection);
		basicSh.setVec2("u_resolution", {1280, 720});
		basicSh.setFloat("u_thickness", 10);

		
		path.trimBegin(begin);
		path.trimEnd(end);
		

		path.draw();
		
		Seden::win::drawGui();
		Seden::win::display();
	}

	Seden::win::terminate();
	Seden::win::closeGui();
	//FT_Done_Face(face);
	//FT_Done_FreeType(library);
}