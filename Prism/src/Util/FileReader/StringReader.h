#pragma once

#include <string>
#include <optional>

namespace Prism {

	class StringReader {
	public:
		inline static std::optional<std::string> ReadFile(const std::string& filepath);
	};
}