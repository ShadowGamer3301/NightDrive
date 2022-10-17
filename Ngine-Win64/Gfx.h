#pragma once
#include "Window.h"
#include <glm/mat4x4.hpp>
#include <vector>

namespace Ngine {

	class NAPI Matrix {
	public:
		void Initialize(GLuint program);

		GLuint matrixID;
		glm::mat4 MVP;
	};

	struct NAPI Object {
		std::vector<float> verticies;
		std::vector<float> color;
		GLuint program;
		GLuint VAO, VBO, CBO;
		Matrix mat;

		void Draw();
		void InitMatrix();
	};

	class NAPI Gfx {
	public:
		static GLuint CompileShader(const char* vpath, const char* fpath);
	};
}

