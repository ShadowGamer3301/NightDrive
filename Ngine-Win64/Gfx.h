#pragma once
#include "Window.h"
#include <glm/mat4x4.hpp>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <tiny_obj_loader.h>
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
		void Tanslate(glm::vec3 v) { mat.MVP = glm::translate(mat.MVP, v); };
	};

	class NAPI Gfx {
	public:
		static GLuint CompileShader(const char* vpath, const char* fpath);
		static GLuint LoadBMP(const char* ipath);
		static GLuint LoadDDS(const char* ipath);
		static void LoadOBJLegacy(const char* opath, std::vector<glm::vec3>& verticies, std::vector<glm::vec2>& uvs, std::vector<glm::vec3>& normals, bool dds);
		static void LoadOBJ(const char* opath, const char* mpath, std::vector<glm::vec3>& verticies, std::vector<glm::vec2>& uvs, std::vector<glm::vec3>& normals);
	};
}

