#pragma once
#include <string>

namespace vulture {

using String = std::string;

template<typename ... Args>
String stringFormat(const String& format, Args ...args)
{
	String s;
	int size = std::snprintf(nullptr, 0, format.c_str(), args ...);
	if (size > 0) 
	{
		s.resize(size);
		std::snprintf(s.data(), static_cast<size_t>(size) + 1, format.c_str(), args ...);
	}
	return s;
}

} // namespace vulture
