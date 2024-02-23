#include <seden.h>
#include <vector>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>


GLuint CreateSSBO(std::vector<glm::vec4>& varray)
{
	GLuint ssbo;
	glGenBuffers(1, &ssbo);
	glBindBuffer(GL_SHADER_STORAGE_BUFFER, ssbo);
	glBufferData(GL_SHADER_STORAGE_BUFFER, varray.size() * sizeof(*varray.data()), varray.data(), GL_STATIC_DRAW);
	return ssbo;
}

int main()
{
	Seden::init(1280, 720, "vector graphics");
	
	Shader sh = Shader("assets/vert.glsl", "assets/frag.glsl");

	GLuint program = sh.getId();
	GLint  loc_mvp = glGetUniformLocation(program, "u_mvp");
	GLint  loc_res = glGetUniformLocation(program, "u_resolution");
	GLint  loc_thi = glGetUniformLocation(program, "u_thickness");

	glUseProgram(program);
	glUniform1f(loc_thi, 20.0);

	GLushort pattern = 0x18ff;
	GLfloat  factor = 2.0f;

	glm::vec4 p0(-1.0f, -1.0f, 0.0f, 1.0f);
	glm::vec4 p1(1.0f, -1.0f, 0.0f, 1.0f);
	glm::vec4 p2(1.0f, 1.0f, 0.0f, 1.0f);
	glm::vec4 p3(-1.0f, 1.0f, 0.0f, 1.0f);
	std::vector<glm::vec4> varray1{ p3, p0, p1, p2, p3, p0, p1 };
	GLuint ssbo1 = CreateSSBO(varray1);

	std::vector<glm::vec4> varray2;
	for (int u = -8; u <= 368; u += 8)
	{
		double a = u * 3.1415965 / 180.0;
		double c = cos(a), s = sin(a);
		varray2.emplace_back(glm::vec4((float)c, (float)s, 0.0f, 1.0f));
	}
	GLuint ssbo2 = CreateSSBO(varray2);

	GLuint vao;
	glGenVertexArrays(1, &vao);
	glBindVertexArray(vao);

	glClearColor(0.0f, 0.0f, 0.0f, 0.0f);
	//glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);

	glm::mat4(project);
	int vpSize[2]{ 0, 0 };
	
	int w = 1280, h=720;
	while (Seden::isRunning())
	{
		Seden::clear();

		
		if (w != vpSize[0] || h != vpSize[1])
		{
			vpSize[0] = w; vpSize[1] = h;
			glViewport(0, 0, vpSize[0], vpSize[1]);
			float aspect = (float)w / (float)h;
			project = glm::ortho(-aspect, aspect, -1.0f, 1.0f, -10.0f, 10.0f);
			glUniform2f(loc_res, (float)w, (float)h);
		}

		glClear(GL_COLOR_BUFFER_BIT);

		glm::mat4 modelview1(1.0f);
		modelview1 = glm::translate(modelview1, glm::vec3(-0.6f, 0.0f, 0.0f));
		modelview1 = glm::scale(modelview1, glm::vec3(0.5f, 0.5f, 1.0f));
		glm::mat4 mvp1 = project * modelview1;

		glUniformMatrix4fv(loc_mvp, 1, GL_FALSE, glm::value_ptr(mvp1));
		glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 0, ssbo1);
		GLsizei N1 = (GLsizei)varray1.size() - 2;
		glDrawArrays(GL_TRIANGLES, 0, 6 * (N1 - 1));

		glm::mat4 modelview2(1.0f);
		modelview2 = glm::translate(modelview2, glm::vec3(0.6f, 0.0f, 0.0f));
		modelview2 = glm::scale(modelview2, glm::vec3(0.5f, 0.5f, 1.0f));
		glm::mat4 mvp2 = project * modelview2;

		glUniformMatrix4fv(loc_mvp, 1, GL_FALSE, glm::value_ptr(mvp2));
		glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 0, ssbo2);
		GLsizei N2 = (GLsizei)varray2.size() - 2;
		glDrawArrays(GL_TRIANGLES, 0, 6 * (N2 - 1));

		Seden::display();
	}

	Seden::terminate();
}