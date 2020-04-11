#pragma once

#include "Scripting/Lua.h"

#include <string>
#include <unordered_map>

namespace Prism {

	class Settings {
		static void Load(const std::string& filepath);
	};
}