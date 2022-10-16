#pragma once
#include "Macro.h"
#include <exception>
#include <sstream>

namespace Ngine {
	class NAPI Exception : public std::exception {
	public:
		Exception(int line, std::string file, std::string desc);
		const char* what() const noexcept override; //Function that returns information about error

	private:
		std::string file, desc;
		mutable std::string wBuffer;
		int line;
	};
}