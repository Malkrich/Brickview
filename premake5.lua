workspace "Brickview"
    architecture "x86"

    configurations
    {
        "Debug",
        "Release"
    }

    flags
	{
		"MultiProcessorCompile"
	}

outputDir = "%{cfg.buildcfg}-%{cfg.system}"

project "Brickview"
    location "Brickview/Brickview"
    kind "ConsoleApp"
    language "C++"
    cppdialect "C++17"
    systemversion "latest"

    targetdir("bin/" .. outputDir .. "/%{prj.name}")
    objdir("bin-int/" .. outputDir .. "/%{prj.name}")

    includedirs
    {
        "Brickview/Brickview/src",
        "Brickview/vendors/GLFW/glfw-3.3.8/include",
        "Brickview/vendors/glad/include",
        "Brickview/vendors/spdlog/include"
    }

    files
    {
        "Brickview/Brickview/src/**.h",
        "Brickview/Brickview/src/**.cpp"
    }

    links
    {
        "GLFW",
        "glad",
        "spdlog"
    }

    filter "configurations:Debug"
        defines "BV_DEBUG"
        runtime "Debug"

    filter "configurations:Release"
        defines "BV_RELEASE"
        runtime "Release"
        optimize "on"

group "Dependencies"
    include "Brickview/vendors/GLFW"
    include "Brickview/vendors/glad"
    include "Brickview/vendors/spdlog"