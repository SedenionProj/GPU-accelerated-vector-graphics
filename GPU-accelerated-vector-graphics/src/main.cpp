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

struct Character {
	unsigned int TextureID;  // ID handle of the glyph texture
	glm::ivec2   Size;       // Size of glyph
	glm::ivec2   Bearing;    // Offset from baseline to left/top of glyph
	unsigned int Advance;    // Offset to advance to next glyph
};

std::map<char, Character> Characters;

unsigned int VAO, VBO;

void RenderText(Shader& s, std::string text, float x, float y, float scale, glm::vec3 color)
{
	// activate corresponding render state	
	s.Bind();
	glUniform3f(glGetUniformLocation(s.getId(), "textColor"), color.x, color.y, color.z);
	glActiveTexture(GL_TEXTURE0);
	glBindVertexArray(VAO);

	// iterate through all characters
	std::string::const_iterator c;
	for (c = text.begin(); c != text.end(); c++)
	{
		Character ch = Characters[*c];

		float xpos = x + ch.Bearing.x * scale;
		float ypos = y - (ch.Size.y - ch.Bearing.y) * scale;

		float w = ch.Size.x * scale;
		float h = ch.Size.y * scale;
		// update VBO for each character
		float vertices[6][4] = {
			{ xpos,     ypos + h,   0.0f, 0.0f },
			{ xpos,     ypos,       0.0f, 1.0f },
			{ xpos + w, ypos,       1.0f, 1.0f },

			{ xpos,     ypos + h,   0.0f, 0.0f },
			{ xpos + w, ypos,       1.0f, 1.0f },
			{ xpos + w, ypos + h,   1.0f, 0.0f }
		};
		// render glyph texture over quad
		glBindTexture(GL_TEXTURE_2D, ch.TextureID);
		// update content of VBO memory
		glBindBuffer(GL_ARRAY_BUFFER, VBO);
		glBufferSubData(GL_ARRAY_BUFFER, 0, sizeof(vertices), vertices);
		glBindBuffer(GL_ARRAY_BUFFER, 0);
		// render quad
		glDrawArrays(GL_TRIANGLES, 0, 6);
		// now advance cursors for next glyph (note that advance is number of 1/64 pixels)
		x += (ch.Advance >> 6) * scale; // bitshift by 6 to get value in pixels (2^6 = 64)
	}
	glBindVertexArray(0);
	glBindTexture(GL_TEXTURE_2D, 0);
}

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

	std::cout << face->num_glyphs <<"\n";

	FT_Set_Pixel_Sizes(face, 0, 48*10);

	FT_Load_Char(face, '.', FT_LOAD_RENDER);

	std::cout << face->glyph->bitmap.width << "\n";


	glPixelStorei(GL_UNPACK_ALIGNMENT, 1); // disable byte-alignment restriction

	for (unsigned char c = 0; c < 128; c++)
	{
		// load character glyph 
		if (FT_Load_Char(face, c, FT_LOAD_RENDER))
		{
			std::cout << "ERROR::FREETYTPE: Failed to load Glyph" << std::endl;
			continue;
		}
		// generate texture
		unsigned int texture;
		glGenTextures(1, &texture);
		glBindTexture(GL_TEXTURE_2D, texture);
		glTexImage2D(
			GL_TEXTURE_2D,
			0,
			GL_RED,
			face->glyph->bitmap.width,
			face->glyph->bitmap.rows,
			0,
			GL_RED,
			GL_UNSIGNED_BYTE,
			face->glyph->bitmap.buffer
		);
		// set texture options
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
		// now store character for later use
		Character character = {
			texture,
			glm::ivec2(face->glyph->bitmap.width, face->glyph->bitmap.rows),
			glm::ivec2(face->glyph->bitmap_left, face->glyph->bitmap_top),
			face->glyph->advance.x
		};
		Characters.insert(std::pair<char, Character>(c, character));
	}

	glm::mat4 projection = glm::ortho(0.0f, 1280.0f, 0.0f, 720.0f);

	
	glGenVertexArrays(1, &VAO);
	glGenBuffers(1, &VBO);
	glBindVertexArray(VAO);
	glBindBuffer(GL_ARRAY_BUFFER, VBO);
	glBufferData(GL_ARRAY_BUFFER, sizeof(float) * 6 * 4, NULL, GL_DYNAMIC_DRAW);
	glEnableVertexAttribArray(0);
	glVertexAttribPointer(0, 4, GL_FLOAT, GL_FALSE, 4 * sizeof(float), 0);
	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glBindVertexArray(0);

	Shader shader("assets/shaders/ttfVert.glsl", "assets/shaders/ttfFrag.glsl");

	

	while (Seden::isRunning())
	{
		Seden::clear();
		Seden::clearGui();

		shader.Bind();
		shader.setMat4("projection", projection);

		RenderText(shader, "This is sample text", 25.0f, 25.0f, 1.0f, glm::vec3(0.5, 0.8f, 0.2f));
		RenderText(shader, "(C) LearnOpenGL.com", 540.0f, 570.0f, 0.5f, glm::vec3(0.3, 0.7f, 0.9f));

		Seden::drawGui();
		Seden::display();
	}

	Seden::terminate();
	Seden::closeGui();
	FT_Done_Face(face);
	FT_Done_FreeType(library);
}