#include <Ngine.hpp>

int main(void) try {
	mINI::INIFile file("Game.ini");
	mINI::INIStructure ini;
	if (!file.read(ini))
		throw Ngine::Exception(__LINE__, __FILE__, "Could not read configuration file");

	Ngine::Window wnd(std::stoi(ini["Game"]["Width"]), std::stoi(ini["Game"]["Height"]), "NightDrive test build");

	Ngine::Object triangle;
	triangle.verticies = {
		-1.0f, -1.0f, 0.0f,
		 1.0f, -1.0f, 0.0f,
		 0.0f,  1.0f, 0.0f,
	};
	triangle.program = Ngine::Gfx::CompileShader("Shader/DebugV.glsl", "Shader/DebugF.glsl");
	triangle.InitMatrix();

	while (!wnd.ShouldClose())
	{
		wnd.StartRender();
		triangle.Draw();
		wnd.EndRender();
	}

	return EXIT_SUCCESS;
}
catch (const Ngine::Exception& e) {
	printf("%s", e.what());
	return EXIT_FAILURE;
}