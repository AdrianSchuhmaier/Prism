workspace "Prism"
    architecture "x64"
    startproject "Sandbox"

    configurations {
        "Debug",
        "Release"
    }

    flags
    {
        "MultiProcessorCompile"
    }

    defines {
        "NOMINMAX"
    }

outputdir = "%{cfg.buildcfg}"
includedir = {}

includedir["spdlog"] = "Prism/includes/spdlog/include"
includedir["lua"] = "Prism/includes/lua/include"
includedir["glfw"] = "Prism/includes/glfw/include"
includedir["stb"] = "Prism/includes/stb"

group "Dependencies"
	include "Prism/includes/GLFW"
    --include "Prism/vendor/imgui"
    
group ""

project "Prism"
	location "Prism"
	kind "StaticLib"
    language "C++"
    cppdialect "C++17"
	staticruntime "on"
	systemversion "latest"
	
	targetdir ("bin/" .. outputdir)
	objdir ("bin-int/%{prj.name}-" .. outputdir)

	files {
		"%{prj.name}/src/**.h",
		"%{prj.name}/src/**.cpp"
	}

	includedirs {
        "%{prj.name}/src",
        "%{includedir.spdlog}",
        "%{includedir.glfw}",
        "%{includedir.stb}",
        "%{includedir.lua}"
	}

	links {
        "GLFW",
		"Prism/includes/lua/lua53"
	}
	
	filter "configurations:Debug"
        defines "PR_DEBUG"
        runtime "Debug"
		buildoptions "/MT /await"
		symbols "On"

	filter "configurations:Release"
        defines "PR_RELEASE"
        runtime "Release"
		optimize "On"


project "Sandbox"
	location "Sandbox"
	kind "ConsoleApp"
	language "C++"
	cppdialect "C++17"
	staticruntime "On"
	systemversion "latest"
	
	targetdir ("bin/" .. outputdir)
	objdir ("bin-int/%{prj.name}-" .. outputdir)

	files {
		"%{prj.name}/src/**.h",
		"%{prj.name}/src/**.cpp"
	}

	includedirs {
		"Prism/src",
        "%{includedir.spdlog}",
        "%{includedir.lua}"
	}

	links {
		"Prism"
	}
	
	filter "configurations:Debug"
		defines "PR_DEBUG"
		runtime "Debug"
		buildoptions "/MT /await"
		symbols "On"

	filter "configurations:Release"
		defines "PR_RELEASE"
		runtime "Release"
		optimize "On"