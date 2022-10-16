#pragma once
#include "Window.h"
#include <vector>

namespace Ngine {

	struct NAPI Object {
		std::vector<float> verticies;
		bool Processed;
		GLuint program;
		GLuint VAO, VBO;

		void Draw();
	};

	class NAPI Gfx {
	public:
		static GLuint CompileShader(const char* vpath, const char* fpath);
	};
}

