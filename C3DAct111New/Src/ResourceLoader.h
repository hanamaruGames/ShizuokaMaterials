#pragma once
#include <string>

namespace ResourceLoader {
	void Reset();
	char* Load(std::string finename);
};