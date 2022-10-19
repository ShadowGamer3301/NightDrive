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

void Ngine::Gfx::LoadOBJLegacy(const char* opath, std::vector<glm::vec3>& verticies, std::vector<glm::vec2>& uvs, std::vector<glm::vec3>& normals, bool dds)
{
	spdlog::info("Loading mesh in OBJ format: {}", opath);

	std::vector<unsigned int> vertexIndices, uvIndices, normalIndices;
	std::vector<glm::vec3> temp_vertices;
	std::vector<glm::vec2> temp_uvs;
	std::vector<glm::vec3> temp_normals;

	FILE* file = fopen(opath, "r");
	if (file == NULL) {
		spdlog::error("Could not open {}", opath);
		throw Ngine::Exception(__LINE__, __FILE__, "Could not open mesh file");
	}

	while (1) {

		char lineHeader[128];
		// read the first word of the line
		int res = fscanf(file, "%s", lineHeader);
		if (res == EOF)
			break; // EOF = End Of File. Quit the loop.

		// else : parse lineHeader

		if (strcmp(lineHeader, "v") == 0) {
			glm::vec3 vertex;
			fscanf(file, "%f %f %f\n", &vertex.x, &vertex.y, &vertex.z);
			temp_vertices.push_back(vertex);
		}
		else if (strcmp(lineHeader, "vt") == 0) {
			glm::vec2 uv;
			fscanf(file, "%f %f\n", &uv.x, &uv.y);
			if(dds)
				uv.y = -uv.y; // Invert V coordinate since we will only use DDS texture, which are inverted. Remove if you want to use TGA or BMP loaders.
			temp_uvs.push_back(uv);
		}
		else if (strcmp(lineHeader, "vn") == 0) {
			glm::vec3 normal;
			fscanf(file, "%f %f %f\n", &normal.x, &normal.y, &normal.z);
			temp_normals.push_back(normal);
		}
		else if (strcmp(lineHeader, "f") == 0) {
			std::string vertex1, vertex2, vertex3;
			unsigned int vertexIndex[3], uvIndex[3], normalIndex[3];
			int matches = fscanf(file, "%d/%d/%d %d/%d/%d %d/%d/%d\n", &vertexIndex[0], &uvIndex[0], &normalIndex[0], &vertexIndex[1], &uvIndex[1], &normalIndex[1], &vertexIndex[2], &uvIndex[2], &normalIndex[2]);
			if (matches != 9) {
				spdlog::info("File can't be read by our simple parser");
				throw Ngine::Exception(__LINE__, __FILE__, "Could not prase mesh file");
				fclose(file);
			}
			vertexIndices.push_back(vertexIndex[0]);
			vertexIndices.push_back(vertexIndex[1]);
			vertexIndices.push_back(vertexIndex[2]);
			uvIndices.push_back(uvIndex[0]);
			uvIndices.push_back(uvIndex[1]);
			uvIndices.push_back(uvIndex[2]);
			normalIndices.push_back(normalIndex[0]);
			normalIndices.push_back(normalIndex[1]);
			normalIndices.push_back(normalIndex[2]);
		}
		else {
			// Probably a comment, eat up the rest of the line
			char stupidBuffer[1000];
			fgets(stupidBuffer, 1000, file);
		}
	}

	// For each vertex of each triangle
	for (unsigned int i = 0; i < vertexIndices.size(); i++) {

		// Get the indices of its attributes
		unsigned int vertexIndex = vertexIndices[i];
		unsigned int uvIndex = uvIndices[i];
		unsigned int normalIndex = normalIndices[i];

		// Get the attributes thanks to the index
		// Put the attributes in buffers

		glm::vec3 vertex = temp_vertices[vertexIndex - 1];
		verticies.push_back(vertex);

		if (temp_uvs.size() > 0) {
			glm::vec2 uv = temp_uvs[uvIndex - 1];
			uvs.push_back(uv);
		}

		if (temp_normals.size() > 0) {
			glm::vec3 normal = temp_normals[normalIndex - 1];
			normals.push_back(normal);
		}

	}
	fclose(file);
}

void Ngine::Gfx::LoadOBJ(const char* opath, const char* mpath, std::vector<glm::vec3>& verticies, std::vector<glm::vec2>& uvs, std::vector<glm::vec3>& normals)
{
	spdlog::info("Loading mesh in OBJ format: {}", opath);
	spdlog::info("Loading mesh material in : {}", mpath);

	tinyobj::ObjReaderConfig reader_config;
	reader_config.mtl_search_path = mpath;
	tinyobj::ObjReader reader;

	if (!reader.ParseFromFile(opath, reader_config)) {
		if (!reader.Error().empty())
		{
			spdlog::error("Cannot load OBJ file: {}", reader.Error());
		}
		throw Ngine::Exception(__LINE__, __FILE__, "Could not parse mesh file");
	}

	if (!reader.Warning().empty()) {
		spdlog::warn("TinyObjReader: {}", reader.Warning());
	}

	auto& attrib = reader.GetAttrib();
	auto& shapes = reader.GetShapes();
	auto& materials = reader.GetMaterials();

	std::vector<glm::vec3> vv;
	std::vector<glm::vec2> uvv;
	std::vector<glm::vec3> nv;

	// Loop over shapes
	for (size_t s = 0; s < shapes.size(); s++) {
		// Loop over faces(polygon)
		size_t index_offset = 0;
		for (size_t f = 0; f < shapes[s].mesh.num_face_vertices.size(); f++) {
			size_t fv = size_t(shapes[s].mesh.num_face_vertices[f]);
			// Loop over vertices in the face.
			for (size_t v = 0; v < fv; v++) {
				// access to vertex
				tinyobj::index_t idx = shapes[s].mesh.indices[index_offset + v];
				glm::vec3 vert;
				vert.x = attrib.vertices[3 * size_t(idx.vertex_index) + 0];
				vert.y = attrib.vertices[3 * size_t(idx.vertex_index) + 1];
				vert.z = attrib.vertices[3 * size_t(idx.vertex_index) + 2];
				vv.push_back(vert);

				// Check if `normal_index` is zero or positive. negative = no normal data
				if (idx.normal_index >= 0) {
					glm::vec3 norm;
					norm.x = attrib.normals[3 * size_t(idx.normal_index) + 0];
					norm.y = attrib.normals[3 * size_t(idx.normal_index) + 1];
					norm.z = attrib.normals[3 * size_t(idx.normal_index) + 2];
					nv.push_back(norm);
				}

				// Check if `texcoord_index` is zero or positive. negative = no texcoord data
				if (idx.texcoord_index >= 0) {
					glm::vec2 uv;
					uv.x = attrib.texcoords[2 * size_t(idx.texcoord_index) + 0];
					uv.y = attrib.texcoords[2 * size_t(idx.texcoord_index) + 1];
					uvv.push_back(uv);
				}

				// Optional: vertex colors
				// tinyobj::real_t red   = attrib.colors[3*size_t(idx.vertex_index)+0];
				// tinyobj::real_t green = attrib.colors[3*size_t(idx.vertex_index)+1];
				// tinyobj::real_t blue  = attrib.colors[3*size_t(idx.vertex_index)+2];
			}
			index_offset += fv;

			// per-face material
			shapes[s].mesh.material_ids[f];
		}
	}

	verticies = vv;
	normals = nv;
	uvs = uvv;
}

void Ngine::Object::Draw()
{

	//Enable associated program
	glUseProgram(program);

	glUniformMatrix4fv(mat.matrixID, 1, GL_FALSE, &mat.MVP[0][0]);

	//Generate VAO
	glGenVertexArrays(1, &VAO);
	glBindVertexArray(VAO);

	//Generate VBO
	glGenBuffers(1, &VBO);
	glBindBuffer(GL_ARRAY_BUFFER, VBO);
	glBufferData(GL_ARRAY_BUFFER, sizeof(glm::vec3) * verticies.size(), verticies.data(), GL_STATIC_DRAW);
	
	//Generate CBO
	if (!color.empty()) {
		glGenBuffers(1, &CBO);
		glBindBuffer(GL_ARRAY_BUFFER, CBO);
		glBufferData(GL_ARRAY_BUFFER, sizeof(glm::vec3) * color.size(), color.data(), GL_STATIC_DRAW);
	}

	//Generate UBO
	if (!uvs.empty()) {
		glGenBuffers(1, &UBO);
		glBindBuffer(GL_ARRAY_BUFFER, UBO);
		glBufferData(GL_ARRAY_BUFFER, sizeof(glm::vec2) * uvs.size(), uvs.data(), GL_STATIC_DRAW);
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

	mINI::INIFile file("Game.ini");
	mINI::INIStructure ini;
	if (!file.read(ini))
		throw Ngine::Exception(__LINE__, __FILE__, "Could not read configuration file");

	//45° Field of View, dynamic ratio, display range : 0.1 unit <-> 100 units
	glm::mat4 Projection = glm::perspective(glm::radians(90.0f), (float)std::stoi(ini["Game"]["Width"]) / (float)std::stoi(ini["Game"]["Height"]), 0.1f, 100.0f);

	//Camera matrix
	glm::mat4 View = glm::lookAt(glm::vec3(4, 3, 3), glm::vec3(0, 0, 0), glm::vec3(0, 1, 0));

	//Model matrix
	glm::mat4 Model = glm::mat4(1.0f);

	MVP = Projection * View * Model;
}
