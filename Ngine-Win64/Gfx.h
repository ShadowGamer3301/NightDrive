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
		std::vector<glm::vec3> verticies;
		std::vector<glm::vec3> color;
		std::vector<glm::vec3> normals;
		std::vector<glm::vec2> uvs;
		GLuint program, texture;
		GLuint VAO, VBO, CBO, UBO;
		Matrix mat;

		void Draw();
		void InitMatrix();
	};

	class NAPI Gfx {
	public:
		static GLuint CompileShader(const char* vpath, const char* fpath);
		static GLuint LoadBMP(const char* ipath);
		static GLuint LoadDDS(const char* ipath);
		static void LoadOBJ(const char* opath, std::vector<glm::vec3>& verticies, std::vector<glm::vec2>& uvs, std::vector<glm::vec3>& normals, bool dds);
	};
}

