#include "pch.h"
#include "Exception.h"

Ngine::Exception::Exception(int line, std::string file, std::string desc) : line(line), file(file), desc(desc)
{
}

const char* Ngine::Exception::what() const noexcept
{
    std::ostringstream oss;
    oss << "Exception caught!" << std::endl
        << "Line: " << line << std::endl
        << "File: " << file << std::endl
        << "Desc: " << desc << std::endl;

    wBuffer = oss.str();
    return wBuffer.c_str();
}
