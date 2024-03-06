#pragma once


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

	Path(const std::vector<glm::vec4>& vert, float tickness){
		
		init(vert);
	}

	void draw() {
		glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 0, ssbo);
		int nb_lines = getSegmentNumber();
		int triangles_end_nb = 6 * int(floor(nb_lines * tEnd) + 1);
		int triangles_begin_nb = 6 * int(floor(nb_lines * tBegin));
		glDrawArrays(GL_TRIANGLES, triangles_begin_nb, triangles_end_nb - triangles_begin_nb);
	}

	void update(int pos, const glm::vec4& data) {
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

				glm::vec3 p1 = vertices[i];
				glm::vec3 p2 = vertices[i + 1];
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
			glm::vec4 interpolated = (1.f - seg_t) * vertices[line1] + (seg_t)*vertices[line2];
			
			updatePath(lastTend, vertices[lastTend]);
			updatePath(line2, interpolated);
			
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

				glm::vec3 p1 = vertices[i];
				glm::vec3 p2 = vertices[i + 1];
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
			glm::vec4 interpolated = (1.f - seg_t) * vertices[line1] + (seg_t)*vertices[line2];

			updatePath(lastTbegin, vertices[lastTbegin]);
			updatePath(line1, interpolated);
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

	void init(const std::vector<glm::vec4>& vert) {
		vertices = vert;
		ssbo = CreateSSBO(vert);
		computeLength();
	}

	void computeLength() {
		for (int i = 1; i < vertices.size() - 2; i++) {
			glm::vec3 seg = vertices[i + 1] - vertices[i];
			pathLength += glm::length(seg);
		}
	}

	void updatePath(int pos, const glm::vec4& data) {
		glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 0, ssbo);
		glBufferSubData(GL_SHADER_STORAGE_BUFFER, sizeof(glm::vec4) * pos, sizeof(glm::vec4), glm::value_ptr(data));
	}
	std::vector<glm::vec4> vertices;
	int ssbo;
};

class QuadraticBezier : public Path {
public:
	QuadraticBezier(glm::vec4 p0, glm::vec4 p1, glm::vec4 p2) {
		std::vector<glm::vec4> vert;
		float subdivisions = 0.05;
		for (float t = -subdivisions; t <= 1 + subdivisions*2; t += subdivisions) {

			glm::vec4 pos = (1 - t) * (1 - t) * p0 + (1 - t) * 2 * t * p1 + t * t * p2;
			vert.push_back(pos);
		}
		
		init(vert);
	}
};