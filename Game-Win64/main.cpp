#include <Ngine.hpp>

int main(void) try {
	mINI::INIFile file("Game.ini");
	mINI::INIStructure ini;
	if (!file.read(ini))
		throw Ngine::Exception(__LINE__, __FILE__, "Could not read configuration file");

	Ngine::Window wnd(std::stoi(ini["Game"]["Width"]), std::stoi(ini["Game"]["Height"]), "NightDrive test build");

	Ngine::Object obj;
	

	obj.program = Ngine::Gfx::CompileShader("Shader/TTV.glsl", "Shader/TTF.glsl");
	Ngine::Gfx::LoadOBJ("Test.obj", ".", obj.verticies, obj.uvs, obj.normals);
	obj.texture = Ngine::Gfx::LoadBMP("road.bmp");
	obj.InitMatrix();

	while (!wnd.ShouldClose())
	{
		wnd.StartRender();
		obj.Tanslate(glm::vec3(0.0f, -0.001f, 0.0f));
		obj.Draw();
		wnd.EndRender();
	}

	return EXIT_SUCCESS;
}
catch (const Ngine::Exception& e) {
	printf("%s", e.what());
	return EXIT_FAILURE;
}