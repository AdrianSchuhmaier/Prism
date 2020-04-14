#pragma once

#include "Util/Log/Log.h"
#include "Util/FileReader/StringReader.h"

#include <shaderc/shaderc.hpp>
#include <unordered_map>
#include <fstream>
#include <sstream>
#include <streambuf>

namespace Prism {

	enum class ShaderType {
		Vertex, Fragment, Compute, Geometry, TesselationControl, TesselationEvaluation
	};

	using ShaderCode = std::unordered_map<ShaderType, std::string>;
	using ShaderBinary = std::unordered_map<ShaderType, std::vector<uint32_t>>;

	class ShaderUtil {
	public:
		std::optional<ShaderBinary> Load(const std::string& filepath);
		std::optional<ShaderBinary> Compile(const ShaderCode& code, const char* name);
		std::optional<ShaderCode> ReadFile(const std::string& filepath);
	};


	std::optional<ShaderBinary> ShaderUtil::Load(const std::string& filepath)
	{
		auto code = ReadFile(filepath);
		if (!code.has_value()) return std::nullopt;


		const char* name = "shader_without_name";
		size_t pos = filepath.find_last_of('/');
		if (pos != std::string::npos)
			name = filepath.substr(pos, filepath.size() - pos).c_str();

		return Compile(code.value(), name);
	}

	std::optional<ShaderCode> ShaderUtil::ReadFile(const std::string& filepath)
	{
		auto fileContent = StringReader::ReadFile(filepath);
		if (!fileContent.has_value())
		{
			PR_CORE_ERROR("Could not load shader {0}", filepath);
			return std::nullopt;
		}

		// split content into shader sources
		ShaderCode result;

		const char* typeToken = "#type";
		size_t typeTokenLength = strlen(typeToken);
		size_t pos = fileContent.value().find(typeToken, 0);
		while (pos != std::string::npos)
		{
			// read type
			size_t eol = fileContent.value().find_first_of("\r\n", pos);
			PR_CORE_ASSERT(eol != std::string::npos, "Syntax error");
			size_t begin = pos + typeTokenLength + 1;
			std::string typestr = fileContent.value().substr(begin, eol - begin);

			// parse type
			ShaderType type;
			if (typestr == "vertex")
				type = ShaderType::Vertex;
			else if (typestr == "fragment")
				type = ShaderType::Fragment;
			else if (typestr == "compute")
				type = ShaderType::Compute;
			else if (typestr == "geometry")
				type = ShaderType::Geometry;
			else if (typestr == "tesselation control")
				type = ShaderType::TesselationControl;
			else if (typestr == "tesselation evaluation")
				type = ShaderType::TesselationEvaluation;
			else
			{
				PR_CORE_ERROR(false, "Unknown shader type '{0}' in {1}", typestr, filepath);
				return std::nullopt;
			}

			// read shader source
			size_t nextLinePos = fileContent.value().find_first_not_of("\r\n", eol);
			pos = fileContent.value().find(typeToken, nextLinePos);
			result[type] = fileContent.value().substr(nextLinePos,
				pos - (nextLinePos == std::string::npos ? fileContent.value().size() - 1 : nextLinePos));
		}

		return result;
	}

	std::optional<ShaderBinary> ShaderUtil::Compile(const ShaderCode& code, const char* name)
	{
		ShaderBinary result;
		shaderc::Compiler compiler;
		shaderc::CompileOptions options;

		options.AddMacroDefinition("MY_DEFINE", "1");
		options.SetOptimizationLevel(shaderc_optimization_level_size);

		for (const auto& shader : code)
		{
			shaderc::SpvCompilationResult compiled = compiler.CompileGlslToSpv(
				shader.second, /* code */
				(shaderc_shader_kind)shader.first, /* type */
				name, options);

			if (compiled.GetCompilationStatus() != shaderc_compilation_status_success) {
				PR_CORE_ERROR("Error compiling shader '{0}': {1}", name, compiled.GetErrorMessage());
				return std::nullopt;
			}
			result[shader.first] = { compiled.cbegin(), compiled.cend() };
		}

		PR_CORE_TRACE("Shader '{0}' compiled", name);
		return result;
	}

}