#include "pch.h"
#include "Gfx.h"
#include <fstream>
#include <spdlog/spdlog.h>
#include <glm/matrix.hpp>
#include <glm/gtc/matrix_transform.hpp>

#define FOURCC_DXT1 0x31545844 // Equivalent to "DXT1" in ASCII
#define FOURCC_DXT3 0x33545844 // Equivalent to "DXT3" in ASCII
#define FOURCC_DXT5 0x35545844 // Equivalent to "DXT5" in ASCII

#pragma warning(disable : 4996)

GLuint Ngine::Gfx::CompileShader(const char* vertex_file_path, const char* fragment_file_path)
{
	// Create the shaders
	GLuint VertexShaderID = glCreateShader(GL_VERTEX_SHADER);
	GLuint FragmentShaderID = glCreateShader(GL_FRAGMENT_SHADER);

	// Read the Vertex Shader code from the file
	std::string VertexShaderCode;
	std::ifstream VertexShaderStream(vertex_file_path, std::ios::in);
	if (VertexShaderStream.is_open()) {
		std::stringstream sstr;
		sstr << VertexShaderStream.rdbuf();
		VertexShaderCode = sstr.str();
		VertexShaderStream.close();
	}
	else {
		spdlog::error("Impossible to open {}. Are you in the right directory ? Don't forget to read the FAQ !", vertex_file_path);
		throw Ngine::Exception(__LINE__, __FILE__, "Could not open shader file");
	}

	// Read the Fragment Shader code from the file
	std::string FragmentShaderCode;
	std::ifstream FragmentShaderStream(fragment_file_path, std::ios::in);
	if (FragmentShaderStream.is_open()) {
		std::stringstream sstr;
		sstr << FragmentShaderStream.rdbuf();
		FragmentShaderCode = sstr.str();
		FragmentShaderStream.close();
	}
	else {
		spdlog::error("Impossible to open {}. Are you in the right directory ? Don't forget to read the FAQ !", vertex_file_path);
		throw Ngine::Exception(__LINE__, __FILE__, "Could not open shader file");
	}

	GLint Result = GL_FALSE;
	int InfoLogLength;


	// Compile Vertex Shader
	spdlog::info("Compiling shader: {}", vertex_file_path);
	char const* VertexSourcePointer = VertexShaderCode.c_str();
	glShaderSource(VertexShaderID, 1, &VertexSourcePointer, NULL);
	glCompileShader(VertexShaderID);

	// Check Vertex Shader
	glGetShaderiv(VertexShaderID, GL_COMPILE_STATUS, &Result);
	glGetShaderiv(VertexShaderID, GL_INFO_LOG_LENGTH, &InfoLogLength);
	if (InfoLogLength > 0) {
		std::vector<char> VertexShaderErrorMessage(InfoLogLength + 1);
		glGetShaderInfoLog(VertexShaderID, InfoLogLength, NULL, &VertexShaderErrorMessage[0]);
		spdlog::error("{}", &VertexShaderErrorMessage[0]);
	}



	// Compile Fragment Shader
	spdlog::info("Compiling shader: {}", fragment_file_path);
	char const* FragmentSourcePointer = FragmentShaderCode.c_str();
	glShaderSource(FragmentShaderID, 1, &FragmentSourcePointer, NULL);
	glCompileShader(FragmentShaderID);

	// Check Fragment Shader
	glGetShaderiv(FragmentShaderID, GL_COMPILE_STATUS, &Result);
	glGetShaderiv(FragmentShaderID, GL_INFO_LOG_LENGTH, &InfoLogLength);
	if (InfoLogLength > 0) {
		std::vector<char> FragmentShaderErrorMessage(InfoLogLength + 1);
		glGetShaderInfoLog(FragmentShaderID, InfoLogLength, NULL, &FragmentShaderErrorMessage[0]);
		spdlog::error("{}", &FragmentShaderErrorMessage[0]);
	}



	// Link the program
	spdlog::info("Linking program");
	GLuint ProgramID = glCreateProgram();
	glAttachShader(ProgramID, VertexShaderID);
	glAttachShader(ProgramID, FragmentShaderID);
	glLinkProgram(ProgramID);

	// Check the program
	glGetProgramiv(ProgramID, GL_LINK_STATUS, &Result);
	glGetProgramiv(ProgramID, GL_INFO_LOG_LENGTH, &InfoLogLength);
	if (InfoLogLength > 0) {
		std::vector<char> ProgramErrorMessage(InfoLogLength + 1);
		glGetProgramInfoLog(ProgramID, InfoLogLength, NULL, &ProgramErrorMessage[0]);
		spdlog::error("{}", &ProgramErrorMessage[0]);
	}


	glDetachShader(ProgramID, VertexShaderID);
	glDetachShader(ProgramID, FragmentShaderID);

	glDeleteShader(VertexShaderID);
	glDeleteShader(FragmentShaderID);

	return ProgramID;
}

GLuint Ngine::Gfx::LoadBMP(const char* ipath)
{
	spdlog::info("Loading texture in BMP format: {}", ipath);

	//Date read from the header of the BMP file
	unsigned char header[54];
	unsigned int dataPos;
	unsigned int imageSize;
	unsigned int width, height;

	//Actual RGB data
	unsigned char* data;

	//Open the file
	FILE* file = fopen(ipath, "rb");
	if (!file) {
		spdlog::error("Could not open {}", ipath);
		throw Ngine::Exception(__LINE__, __FILE__, "Could not open texture file");
	}

	//If file contain less than 54 bytes of data it's corrupted
	if (fread(header, 1, 54, file) != 54) {
		spdlog::error("{} is corrupted", ipath);
		throw Ngine::Exception(__LINE__, __FILE__, "Could not handle texture file");
	}

	//BMP file always starts with letters BM
	if (header[0] != 'B' || header[1] != 'M') {
		spdlog::error("{} is corrupted", ipath);
		throw Ngine::Exception(__LINE__, __FILE__, "Could not handle texture file");
	}

	// Make sure this is a 24bpp file
	if (*(int*)&(header[0x1E]) != 0) {
		spdlog::error("{} is corrupted", ipath);
		throw Ngine::Exception(__LINE__, __FILE__, "Could not handle texture file");
	}

	if (*(int*)&(header[0x1C]) != 24) {
		spdlog::error("{} is corrupted", ipath);
		throw Ngine::Exception(__LINE__, __FILE__, "Could not handle texture file");
	}

	// Read the information about the image
	dataPos = *(int*)&(header[0x0A]);
	imageSize = *(int*)&(header[0x22]);
	width = *(int*)&(header[0x12]);
	height = *(int*)&(header[0x16]);

	// Some BMP files are misformatted, guess missing information
	if (imageSize == 0)    imageSize = width * height * 3; // 3 : one byte for each Red, Green and Blue component
	if (dataPos == 0)      dataPos = 54; // The BMP header is done that way

	// Create a buffer
	data = new unsigned char[imageSize];
	fread(data, 1, imageSize, file);

	fclose(file);

	// Create one OpenGL texture
	GLuint textureID;
	glGenTextures(1, &textureID);

	// "Bind" the newly created texture : all future texture functions will modify this texture
	glBindTexture(GL_TEXTURE_2D, textureID);

	// Give the image to OpenGL
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_BGR, GL_UNSIGNED_BYTE, data);

	// OpenGL has now copied the data. Free our own version
	delete[] data;

	//Enable trilinear filtering
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);

	//Generate mipmaps
	glGenerateMipmap(GL_TEXTURE_2D);
	return textureID;
}

GLuint Ngine::Gfx::LoadDDS(const char* ipath)
{
	unsigned char header[124];

	FILE* fp;

	//Open texture file
	fp = fopen(ipath, "rb");
	if (fp == NULL) {
		spdlog::error("Could not open {}", ipath);
		throw Ngine::Exception(__LINE__, __FILE__, "Could not open texture file");
	}

	char filecode[4];
	fread(filecode, 1, 4, fp);
	if (strncmp(filecode, "DDS ", 4) != 0) {
		spdlog::error("{} is corrupted", ipath);
		throw Ngine::Exception(__LINE__, __FILE__, "Could not handle texture file");
	}

	fread(&header, 124, 1, fp);

	unsigned int height = *(unsigned int*)&(header[8]);
	unsigned int width = *(unsigned int*)&(header[12]);
	unsigned int linearSize = *(unsigned int*)&(header[16]);
	unsigned int mipMapCount = *(unsigned int*)&(header[24]);
	unsigned int fourCC = *(unsigned int*)&(header[80]);
	unsigned char* buffer;
	unsigned int bufsize;

	//Read mipmap levels
	bufsize = mipMapCount > 1 ? linearSize * 2 : linearSize;
	buffer = (unsigned char*)malloc(bufsize * sizeof(unsigned char));
	fread(buffer, 1, bufsize, fp);
	fclose(fp);

	unsigned int components = (fourCC == FOURCC_DXT1) ? 3 : 4;
	unsigned int format;

	//Try to detect format
	switch (fourCC)
	{
	case FOURCC_DXT1:
		format = GL_COMPRESSED_RGBA_S3TC_DXT1_EXT;
		break;
	case FOURCC_DXT3:
		format = GL_COMPRESSED_RGBA_S3TC_DXT3_EXT;
		break;
	case FOURCC_DXT5:
		format = GL_COMPRESSED_RGBA_S3TC_DXT5_EXT;
		break;
	default:
		spdlog::error("Could not establish format of {}", ipath);
		free(buffer);
		throw Ngine::Exception(__LINE__, __FILE__, "Could not handle texture file");
	}

	// Create one OpenGL texture
	GLuint textureID;
	glGenTextures(1, &textureID);

	// "Bind" the newly created texture : all future texture functions will modify this texture
	glBindTexture(GL_TEXTURE_2D, textureID);
	glPixelStorei(GL_UNPACK_ALIGNMENT, 1);

	unsigned int blockSize = (format == GL_COMPRESSED_RGBA_S3TC_DXT1_EXT) ? 8 : 16;
	unsigned int offset = 0;

	/* load the mipmaps */
	for (unsigned int level = 0; level < mipMapCount && (width || height); ++level)
	{
		unsigned int size = ((width + 3) / 4) * ((height + 3) / 4) * blockSize;
		glCompressedTexImage2D(GL_TEXTURE_2D, level, format, width, height,
			0, size, buffer + offset);

		offset += size;
		width /= 2;
		height /= 2;

		// Deal with Non-Power-Of-Two textures. This code is not included in the webpage to reduce clutter.
		if (width < 1) width = 1;
		if (height < 1) height = 1;

	}

	free(buffer);

	return textureID;
}

void Ngine::Object::Draw()
{
	float* vert = verticies.data();

	//Enable associated program
	glUseProgram(program);

	glUniformMatrix4fv(mat.matrixID, 1, GL_FALSE, &mat.MVP[0][0]);

	//Generate VAO
	glGenVertexArrays(1, &VAO);
	glBindVertexArray(VAO);

	//Generate VBO
	glGenBuffers(1, &VBO);
	glBindBuffer(GL_ARRAY_BUFFER, VBO);
	glBufferData(GL_ARRAY_BUFFER, sizeof(float) * verticies.size(), verticies.data(), GL_STATIC_DRAW);
	
	//Generate CBO
	if (!color.empty()) {
		glGenBuffers(1, &CBO);
		glBindBuffer(GL_ARRAY_BUFFER, CBO);
		glBufferData(GL_ARRAY_BUFFER, sizeof(float) * color.size(), color.data(), GL_STATIC_DRAW);
	}

	//Generate UBO
	if (!uvs.empty()) {
		glGenBuffers(1, &UBO);
		glBindBuffer(GL_ARRAY_BUFFER, UBO);
		glBufferData(GL_ARRAY_BUFFER, sizeof(float) * uvs.size(), uvs.data(), GL_STATIC_DRAW);
	}

	//Draw object
	glEnableVertexAttribArray(0);
	glBindBuffer(GL_ARRAY_BUFFER, VBO);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, (void*)0);

	if (!color.empty()) {
		glEnableVertexAttribArray(1);
		glBindBuffer(GL_ARRAY_BUFFER, CBO);
		glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 0, (void*)0);
	}

	if (!uvs.empty()) {
		glEnableVertexAttribArray(2);
		glBindBuffer(GL_ARRAY_BUFFER, UBO);
		glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 0, (void*)0);
	}

	glDrawArrays(GL_TRIANGLES, 0, verticies.size());
	glDisableVertexAttribArray(0);	

	//Delete buffers
	if(!uvs.empty())
		glDeleteBuffers(1, &UBO);

	if(!color.empty())
		glDeleteBuffers(1, &CBO);

	glDeleteBuffers(1, &VBO);
	glDeleteVertexArrays(1, &VAO);
}

void Ngine::Object::InitMatrix()
{
	mat.Initialize(program);
}

void Ngine::Matrix::Initialize(GLuint program)
{
	matrixID = glGetUniformLocation(program, "MVP");

	//45° Field of View, 4:3 ratio, display range : 0.1 unit <-> 100 units
	glm::mat4 Projection = glm::perspective(glm::radians(45.0f), 4.0f / 3.0f, 0.1f, 100.0f);

	//Camera matrix
	glm::mat4 View = glm::lookAt(glm::vec3(4, 3, 3), glm::vec3(0, 0, 0), glm::vec3(0, 1, 0));

	//Model matrix
	glm::mat4 Model = glm::mat4(1.0f);

	MVP = Projection * View * Model;
}
