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
	LineStrip() = default;

	LineStrip(const std::vector<glm::vec4>& vert, float tickness) : t(1) {
		vertices = vert;
		ssbo = CreateSSBO(vert);
	}

	void draw() {
		glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 0, ssbo);
		int nb_lines = size() - 3;
		int nb_triangles = 6 * int(floor(nb_lines * t) + 1) ;
		//int nb_triangles_new = nb_triangles % 6 == 0 ? nb_triangles : nb_triangles + 3;
		glDrawArrays(GL_TRIANGLES, 0, nb_triangles);
	}

	void point(int pos, const glm::vec4& data) {
		vertices[pos] = data;
		update(pos, data);
	}

	int size() {
		return vertices.size();
	}

	void trim(float t1) {
		if (t <= 1) {
			t = t1;

			float x = (size() - 3) * t;

			int line1 = int(x)+1;
			int line2 = line1 +1;

			linetest = line1;

			float seg_t = x - floor(x);

			seg2 = seg_t;

			glm::vec4 interpolated = (1.f - seg_t) * vertices[line1] + (seg_t) * vertices[line2];

			update(line2, interpolated);
		}
	}
	float seg2;
	int linetest;

	float t;
protected:
	void update(int pos, const glm::vec4& data) {
		//vertices[pos] = data;
		glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 0, ssbo);
		glBufferSubData(GL_SHADER_STORAGE_BUFFER, sizeof(glm::vec4) * pos, sizeof(glm::vec4), glm::value_ptr(data));
	}
	std::vector<glm::vec4> vertices;
	int ssbo;
};

class QuadraticBezier: public LineStrip {
public:
	QuadraticBezier(glm::vec4 p0, glm::vec4 p1, glm::vec4 p2) {
		std::vector<glm::vec4> vert;
		for (float t = 0; t < 1; t += 0.1) {
			glm::vec4 pos = (1 - t) * (1 - t) * p0 + (1 - t) * 2 * t * p1 + t * t * p2;
			vert.push_back(pos);
		}

		t = 1;
		vertices = vert;
		ssbo = CreateSSBO(vert);
	}
};

int main()
{
	Seden::init(1280, 720, "vector graphics");
	Seden::initGui();

	Shader sh = Shader("assets/vert.glsl", "assets/frag.glsl");
	std::vector<glm::vec4> varray1;

	for (float i = -1; i < 1; i += 0.01) {
		varray1.push_back({ i,0,0,1 });
	}
	
	LineStrip lines(varray1,10.);

	std::vector<glm::vec4> varray2 = { glm::vec4(-1,0,0,1),glm::vec4(-0.5,0,0,1),glm::vec4(0.5,0.0,0,1),glm::vec4(0.5,-0.5,0,1),glm::vec4(0.5,-0.6,0,1) };
	LineStrip segment(varray2,10.);

	QuadraticBezier bezier(glm::vec4( 0,0,0,1 ), glm::vec4( 0.5,0,0,1 ), glm::vec4(1, 1, 0, 1));

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
		Seden::clearGui();

		ImGui::Begin("test");
		ImGui::Text(std::to_string(segment.t).c_str());
		ImGui::Text(std::to_string(segment.linetest).c_str());
		ImGui::Text(std::to_string(segment.seg2).c_str());
		ImGui::End();

		
		glm::vec4 corner = { 1., -glfwGetTime(),0,1 };
		project = glm::ortho(-aspect, aspect, -1.0f, 1.0f, -5.0f, 10.0f);
		glm::mat4 modelview1(1.0f);
		glm::mat4 mvp1 = project * modelview1;


		for (int i = 0; i < lines.size(); i++) {
			lines.point(i, {(1.5*i/lines.size() - 1)*1.5, sin(glfwGetTime()+i/10.f)/5.f, 0, 1});
		}
		lines.trim(glfwGetTime() / 10.);
		bezier.trim(glfwGetTime() / 10.);
		segment.trim(glfwGetTime() / 10.);

		sh.setMat4("u_mvp", mvp1);
		sh.setVec2("u_resolution", glm::vec2(w, h));
		sh.setFloat("u_thickness", 20.f);
		sh.Bind();

		lines.draw();
		bezier.draw();
		segment.draw();

		Seden::drawGui();
		Seden::display();
	}

	Seden::terminate();
	Seden::closeGui();
}