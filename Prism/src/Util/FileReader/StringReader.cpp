#include "StringReader.h"

#include "Util/Log/Log.h"
#include <fstream>

namespace Prism {

	std::optional<std::string> StringReader::ReadFile(const std::string& filepath)
	{
		std::string result;
		std::ifstream in(filepath, std::ios::in | std::ios::binary);
		if (in)
		{
			in.seekg(0, std::ios::end);
			result.resize(in.tellg());
			in.seekg(0, std::ios::beg);
			in.read(&result[0], result.size());
			in.close();
		}
		else
		{
			PR_CORE_ERROR("Could not open file '{0}'", filepath);
			return std::nullopt;
		}

		return result;
	}
}