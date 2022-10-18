#include <Ngine.hpp>

int main(void) try {
	mINI::INIFile file("Game.ini");
	mINI::INIStructure ini;
	if (!file.read(ini))
		throw Ngine::Exception(__LINE__, __FILE__, "Could not read configuration file");

	Ngine::Window wnd(std::stoi(ini["Game"]["Width"]), std::stoi(ini["Game"]["Height"]), "NightDrive test build");

	Ngine::Object obj;
	

	obj.program = Ngine::Gfx::CompileShader("Shader/TDV.glsl", "Shader/TDF.glsl");
	Ngine::Gfx::LoadOBJ("Trunk1.obj", "./", obj.verticies, obj.uvs, obj.normals);
	obj.InitMatrix();

	while (!wnd.ShouldClose())
	{
		wnd.StartRender();
		obj.Draw();
		wnd.EndRender();
	}

	return EXIT_SUCCESS;
}
catch (const Ngine::Exception& e) {
	printf("%s", e.what());
	return EXIT_FAILURE;
}