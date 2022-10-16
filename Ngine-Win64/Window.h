#pragma once
#include "Macro.h"
#include <gl/glew.h>
#include <GLFW/glfw3.h>

namespace Ngine {
	class NAPI Window {
	public:
		Window(int width, int height, const char* title);
		~Window();

		//Delete ability to copy class since we will only provide one window for our game
		Window(const Window&) = delete;
		Window operator=(const Window&) = delete;

		inline bool ShouldClose() const noexcept { return glfwWindowShouldClose(m_Wptr); }
		void StartRender();
		void EndRender();

	private:
		GLFWwindow* m_Wptr;
	}; 
}

