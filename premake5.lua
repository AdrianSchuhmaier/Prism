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
includedir["Vulkan"] = (os.getenv("VK_SDK_PATH") .. "/include")
includedir["VulkanShader"] = (os.getenv("VK_SDK_PATH") .. "/shaderc/libshader/include")

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
        "%{includedir.lua}",
        "%{includedir.stb}",
        "%{includedir.glfw}",
        "%{includedir.Vulkan}",
        "%{includedir.VulkanShader}"
	}

	links {
        "GLFW",
		"Prism/includes/lua/lua53",
		(os.getenv("VK_SDK_PATH") .. "/lib/vulkan-1.lib"),
		(os.getenv("VK_SDK_PATH") .. "/lib/shaderc_combined.lib")
	}

	defines {
		"GLFW_INCLUDE_VULKAN",
		"STB_IMAGE_IMPLEMENTATION"
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