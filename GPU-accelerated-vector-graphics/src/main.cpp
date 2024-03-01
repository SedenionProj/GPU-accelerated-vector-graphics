#include <seden.h>
#include <vector>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <ft2build.h>
#include FT_FREETYPE_H
#include <map>
GLuint CreateSSBO(const std::vector<glm::vec4>& varray)
{
	GLuint ssbo;
	glGenBuffers(1, &ssbo);
	glBindBuffer(GL_SHADER_STORAGE_BUFFER, ssbo);
	glBufferData(GL_SHADER_STORAGE_BUFFER, varray.size() * sizeof(*varray.data()), varray.data(), GL_STATIC_DRAW);
	return ssbo;
}


class Path {
public:
	Path() = default;

	Path(const std::vector<glm::vec4>& vert, float tickness) : t(0) {
		vertices = vert;
		ssbo = CreateSSBO(vert);
		pathLength = 0;
		for (int i = 1; i < vertices.size() - 2; i++) {
			glm::vec2 seg = vertices[i + 1] - vertices[i];
			pathLength += glm::sqrt(seg.x*seg.x+seg.y*seg.y);
			std::cout << glm::sqrt(seg.x * seg.x + seg.y * seg.y) << "\n";
		}
	}

	void draw() {
		glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 0, ssbo);
		int nb_lines = size() - 3;
		int nb_triangles = 6 * int(floor(nb_lines * t) + 1);
		//int nb_triangles_new = nb_triangles % 6 == 0 ? nb_triangles : nb_triangles + 3;
		glDrawArrays(GL_TRIANGLES, 0, nb_triangles);
	}

	void update(int pos, const glm::vec4& data) {
		vertices[pos] = data;
		updatePath(pos, data);
	}

	int size() {
		return vertices.size();
	}

	void trim(float t1) {
		if (t <= 1) {
			int seg_i = 0;
			float totalLen = 0;
			float inf = 0;
			float sup = 0;
			for (int i = 1; i < size() - 2; i++) {
				
				glm::vec2 p1 = vertices[i];
				glm::vec2 p2 = vertices[i+1];
				glm::vec2 seg = p2 - p1;
				
				
				float len = glm::sqrt(seg.x * seg.x + seg.y * seg.y)/ pathLength;

				inf = totalLen;
				sup = totalLen + len;

				seg_i += 1;

				if (t1>=inf && t1<=sup) {
					break;
				}
				
				totalLen += len;
			}

			float subdivisions = 1.f / (size() - 3);

			t = ((seg_i-1)+(t1 - inf)/(sup-inf))*subdivisions;



			float x = (size() - 3) * t;

			int line1 = int(x) + 1;
			int line2 = line1 + 1;


			float seg_t = x - floor(x);

			glm::vec4 interpolated = (1.f - seg_t) * vertices[line1] + (seg_t)*vertices[line2];

			updatePath(line2, interpolated);
		}
	}
	float t;
	float pathLength;
protected:
	void updatePath(int pos, const glm::vec4& data) {
		glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 0, ssbo);
		glBufferSubData(GL_SHADER_STORAGE_BUFFER, sizeof(glm::vec4) * pos, sizeof(glm::vec4), glm::value_ptr(data));
	}
	std::vector<glm::vec4> vertices;
	int ssbo;
};

int main()
{
	Seden::init(1280, 720, "vector graphics");
	Seden::initGui();
	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	glEnable(GL_PROGRAM_POINT_SIZE);

	float aspect = 1280.f / 720.f;

	Shader basicSh("assets/shaders/pathVert.glsl", "assets/shaders/pathFrag.glsl");
	auto projection = glm::ortho(-aspect, aspect, -1.0f, 1.0f, -10.0f, 10.0f);

	std::vector<glm::vec4> varray = { {-1,0,0,1}, {-0.9,0,0,1}, {0.5,0,0,1}, {0.5, 0.1, 0, 1}, {1, 0.1, 0, 1}, {1.1,0.1,0,1} };
	Path path(varray, 10);

	GLuint vao;
	glGenVertexArrays(1, &vao);
	glBindVertexArray(vao);

	float t = 0.0;

	float last = 0;

	while (Seden::isRunning())
	{
		float newt = glfwGetTime();
		float dt = newt - last;
		last = newt;


		Seden::clear();
		Seden::clearGui();

		ImGui::Begin("test");
		ImGui::Text(std::to_string(path.t).c_str());
		ImGui::Text(std::to_string(glfwGetTime()).c_str());
		ImGui::End();

		t = glfwGetTime();
		

		basicSh.Bind();
		basicSh.setMat4("u_mvp", projection);
		basicSh.setVec2("u_resolution", {1280, 720});
		basicSh.setFloat("u_thickness", 10);

		path.trim(cos(t)/2.f + 0.5);

		

		path.draw();


		Seden::drawGui();
		Seden::display();
	}

	Seden::terminate();
	Seden::closeGui();
	//FT_Done_Face(face);
	//FT_Done_FreeType(library);
}