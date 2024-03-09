#pragma once

struct vertex {
	glm::vec4 position;
	glm::vec4 color;
};

GLuint CreateSSBO(const std::vector<vertex>& varray)
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
		
	Path(const std::vector<vertex>& vert, float tickness){
		init(vert);
	}

	void draw() {
		if (tPathEnd <= tPathBegin) return; // temp fix
		glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 0, ssbo);
		int nb_lines = getSegmentNumber();
		int triangles_end_nb = 6 * int(floor(nb_lines * tEnd) + 1);
		int triangles_begin_nb = 6 * int(floor(nb_lines * tBegin));
		glDrawArrays(GL_TRIANGLES, triangles_begin_nb, triangles_end_nb - triangles_begin_nb);
	}

	void update(int pos, const vertex& data) { 
		vertices[pos] = data;
		computeLength(); // can be optimized
		updatePath(pos, data);
	}

	int size() {
		return vertices.size();
	}

	int getSegmentNumber() {
		return size() - 3;
	}

	void trimEnd(float end) {
		end = glm::clamp(end, 0.f, 1.f);
		tPathEnd = end;

		if (tPathEnd < tPathBegin) {
			return;
		}

		if (tPathEnd < 1) {
			int segment_i = 0;
			float trimlLen = 0;
			float inf = 0;
			float sup = 0;
			for (int i = 1; i < size() - 2; i++) {

				glm::vec3 p1 = vertices[i].position;
				glm::vec3 p2 = vertices[i + 1].position;
				glm::vec3 seg = p2 - p1;

				float len = glm::length(seg) / pathLength;

				inf = trimlLen;
				sup = trimlLen + len;

				segment_i += 1;

				if (end >= inf && end <= sup) {
					break;
				}

				trimlLen += len;
			}

			float subdivisions = 1.f / getSegmentNumber();

			tEnd = ((segment_i - 1) + (end - inf) / (sup - inf)) * subdivisions;
			
			

			float whichSeg = getSegmentNumber() * tEnd;
			int line1 = floor(whichSeg) + 1;
			int line2 = line1 + 1;
			float seg_t = whichSeg - floor(whichSeg);
			glm::vec4 interpolatedPosition = (1.f - seg_t) * vertices[line1].position + (seg_t)*vertices[line2].position;
			glm::vec4 interpolatedColor = (1.f - seg_t) * vertices[line1].color + (seg_t)*vertices[line2].color;
			
			updatePath(lastTend, vertices[lastTend]);
			updatePath(line2, {interpolatedPosition, interpolatedColor});
			
			lastTend = line2;
		}
	}

	void trimBegin(float begin) {
		begin = glm::clamp(begin, 0.f, 1.f);
		tPathBegin = begin;

		if (tPathEnd < tPathBegin) {
			return;
		}

		
		if (tPathBegin>=0) {
			int segment_i = 0;
			float trimlLen = 0;
			float inf = 0;
			float sup = 0;
			for (int i = 1; i < size() - 2; i++) {

				glm::vec3 p1 = vertices[i].position;
				glm::vec3 p2 = vertices[i + 1].position;
				glm::vec3 seg = p2 - p1;

				float len = glm::length(seg) / pathLength;

				inf = trimlLen;
				sup = trimlLen + len;

				segment_i += 1;

				if (begin >= inf && begin <= sup) {
					break;
				}

				trimlLen += len;
			}

			float subdivisions = 1.f / getSegmentNumber();

			tBegin = ((segment_i - 1) + (begin - inf) / (sup - inf)) * subdivisions;

			

			float whichSeg = getSegmentNumber() * tBegin;
			int line1 = floor(whichSeg) + 1;
			int line2 = line1 + 1;
			float seg_t = whichSeg - floor(whichSeg);
			glm::vec4 interpolatedPosition = (1.f - seg_t) * vertices[line1].position + (seg_t)*vertices[line2].position;
			glm::vec4 interpolatedColor = (1.f - seg_t) * vertices[line1].color + (seg_t)*vertices[line2].color;

			updatePath(lastTbegin, vertices[lastTbegin]);
			updatePath(line1, { interpolatedPosition, interpolatedColor });
			lastTbegin = line1;
		}
	}
	
	float pathLength = 0;

	static Shader shader;

protected:
	// trim path values input by user
	float tPathBegin = 0;
	float tPathEnd = 1;

	// converted trim path values
	float tBegin = 0;
	float tEnd = 1;
	
	// last trim path values for a bug fix
	int lastTbegin = 0;
	int lastTend = 1;

	void init(const std::vector<vertex>& vert) {
		vertices = vert;
		ssbo = CreateSSBO(vert);
		computeLength();
	}

	void computeLength() {
		for (int i = 1; i < vertices.size() - 2; i++) {
			glm::vec3 seg = vertices[i + 1].position - vertices[i].position;
			pathLength += glm::length(seg);
		}
	}

	void updatePath(int pos, const vertex& data) {
		glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 0, ssbo);
		glBufferSubData(GL_SHADER_STORAGE_BUFFER, sizeof(vertex) * pos, sizeof(vertex), &data);
	}
	std::vector<vertex> vertices;
	int ssbo;
};

class Line : public Path{
public:
	Line(const vertex& a, const vertex& b) {
		std::vector<vertex> vert;
		vert.push_back({a.position + (a.position-b.position)});
		vert.push_back(a);
		vert.push_back(b);
		vert.push_back({b.position + (b.position - a.position)});
		init(vert);
	}
};

//class CurvesAndLines : public Path {
//public:
//	CurvesAndLines(std::vector<glm::vec3> points) {
//		std::vector<glm::vec4> vert;
//		
//
//		init(vert);
//	}
//
//	void createQuadraticBezier(std::vector<glm::vec4>& vert, glm::vec4 p0, glm::vec4 p1, glm::vec4 p2) {
//		float subdivisions = 0.05;
//		for (float t = -subdivisions; t <= 1 + subdivisions * 2; t += subdivisions) {
//			glm::vec4 pos = (1 - t) * (1 - t) * p0 + (1 - t) * 2 * t * p1 + t * t * p2;
//			vert.push_back(pos);
//		}
//	}
//};