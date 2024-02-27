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
		for (float t = 0; t < 1; t += 0.01) {
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
	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

	FT_Library library;
	FT_Face face;

	if (FT_Init_FreeType(&library))
		std::cout << "error freetype\n";

	if (FT_New_Face(library, "assets/fonts/arial.ttf", 0, &face))
		std::cout << "error font\n";

	FT_Set_Pixel_Sizes(face, 0, 48);

	FT_Load_Char(face, '%', FT_LOAD_RENDER);

	std::vector<glm::vec2> points;

	auto outline = face->glyph->outline;
	std::cout << outline.n_contours << "\n";
	for (int i = 0; i < outline.n_points; i++) {
		std::cout << outline.points[i].x << " " << outline.points[i].y << "\n";
		points.push_back({ outline.points[i].x/2500.f,outline.points[i].y / 2500.f, });
	}
	
	//points.push_back({ 0, 0 });
	//points.push_back({1, 0});
	//points.push_back({1, 1});

	GLuint vao, vbo;
	glGenVertexArrays(1, &vao);
	glBindVertexArray(vao);

	glGenBuffers(1, &vbo);
	glBindBuffer(GL_ARRAY_BUFFER, vbo);
	glBufferData(GL_ARRAY_BUFFER, sizeof(glm::vec2) * points.size(), &points[0], GL_STATIC_DRAW);

	glVertexAttribPointer(0, 2, GL_FLOAT, GL_FLOAT, sizeof(glm::vec2), (void*)0);
	glEnableVertexAttribArray(0);

	Shader basicSh("assets/shaders/basicVert.glsl", "assets/shaders/basicFrag.glsl");

	auto projection = glm::ortho(0, 1280, 0, 720);

	while (Seden::isRunning())
	{
		Seden::clear();
		Seden::clearGui();

		basicSh.Bind();
		basicSh.setMat4("projection", projection);
		glBindVertexArray(vao);
		glDrawArrays(GL_LINE_STRIP, 0, points.size());

		Seden::drawGui();
		Seden::display();
	}

	Seden::terminate();
	Seden::closeGui();
	FT_Done_Face(face);
	FT_Done_FreeType(library);
}