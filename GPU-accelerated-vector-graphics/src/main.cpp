#include <seden.h>
#include <vector>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>


GLuint CreateSSBO(const std::vector<glm::vec4>& varray)
{
	GLuint ssbo;
	glGenBuffers(1, &ssbo);
	glBindBuffer(GL_SHADER_STORAGE_BUFFER, ssbo);
	glBufferData(GL_SHADER_STORAGE_BUFFER, varray.size() * sizeof(*varray.data()), varray.data(), GL_STATIC_DRAW);
	return ssbo;
}


class LineStrip {
public:
	LineStrip(const std::vector<glm::vec4>& vert, float tickness) {
		vertices = vert;
		ssbo = CreateSSBO(vert);
	}

	void draw() {
		glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 0, ssbo);
		GLsizei N1 = (GLsizei)vertices.size() - 2;
		glDrawArrays(GL_TRIANGLES, 0, 6 * (N1 - 1));
	}

	void update(int pos,const glm::vec4& data) {
		glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 0, ssbo);
		glBufferSubData(GL_SHADER_STORAGE_BUFFER, sizeof(glm::vec4) * (pos), sizeof(glm::vec4), glm::value_ptr(data));
	}

	int size() {
		return vertices.size();
	}

private:
	std::vector<glm::vec4> vertices;
	int ssbo;
};


int main()
{
	Seden::init(1280, 720, "vector graphics");
	
	Shader sh = Shader("assets/vert.glsl", "assets/frag.glsl");
	std::vector<glm::vec4> varray1;

	for (float i = -1; i < 1; i += 0.01) {
		varray1.push_back({ i,0,0,1 });
	}
	
	auto lines = LineStrip(varray1,10.);

	GLuint vao;
	glGenVertexArrays(1, &vao);
	glBindVertexArray(vao);

	//glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);

	glm::mat4(project);
	
	int w = 1280, h=720;
	float aspect = (float)w / (float)h;
	
	while (Seden::isRunning())
	{
		Seden::clear();
		
		glm::vec4 corner = { 1., -glfwGetTime(),0,1 };
		project = glm::ortho(-aspect, aspect, -1.0f, 1.0f, -5.0f, 10.0f);
		glm::mat4 modelview1(1.0f);
		glm::mat4 mvp1 = project * modelview1;


		for (int i = 0; i < lines.size(); i++) {
			lines.update(i, {(2.*i/lines.size() - 1)*2., sin(glfwGetTime()+i/10.f)/5.f, 0, 1});
		}


		sh.setMat4("u_mvp", mvp1);
		sh.setVec2("u_resolution", glm::vec2(w, h));
		sh.setFloat("u_thickness", 10.f);
		sh.Bind();
		lines.draw();

		Seden::display();
	}

	Seden::terminate();
}