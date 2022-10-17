#include "pch.h"
#include "Window.h"

Ngine::Window::Window(int width, int height, const char* title)
{
	mINI::INIFile file("Game.ini");
	mINI::INIStructure ini;
	if(!file.read(ini))
		throw Ngine::Exception(__LINE__, __FILE__, "Could not read configuration file");
	
	if(ini["Game"]["API"] != "OpenGL")
		throw Ngine::Exception(__LINE__, __FILE__, "API has illegal value");

	if (!glfwInit())
		throw Ngine::Exception(__LINE__, __FILE__, "Could not initialize GLFW library");
	
	glfwWindowHint(GLFW_SAMPLES, std::stoi(ini["Game"]["Samples"]));
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3); //Set opengl version to 3.3
	glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

	m_Wptr = glfwCreateWindow(width, height, title, nullptr, nullptr);
	if(!m_Wptr)
		throw Ngine::Exception(__LINE__, __FILE__, "Could not create window");

	glfwMakeContextCurrent(m_Wptr);

	glewExperimental = true; //Enable Opengl experimental functions

	if(glewInit() != GLEW_OK)
		throw Ngine::Exception(__LINE__, __FILE__, "Could not initialize GLEW library");

	glClearColor(0.0f, 0.0f, 0.4f, 1.0f);

	glEnable(GL_DEPTH_TEST);

	glDepthFunc(GL_LESS);
}

Ngine::Window::~Window()
{
	glfwDestroyWindow(m_Wptr);
	glfwTerminate();
}

void Ngine::Window::StartRender()
{
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT); //Clear back buffer data
}

void Ngine::Window::EndRender()
{
	glfwSwapBuffers(m_Wptr); //Move back buffer to front and display it on screen
	glfwPollEvents(); //Check for any input
}
