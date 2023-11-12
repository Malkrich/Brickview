workspace "Brickview"
    architecture "x86"

    configurations
    {
        "Debug",
        "Release"
    }

outputDir = "%{cfg.buildcfg}-%{cfg.system}"

project "spdlog"
    kind "StaticLib"

    targetdir("bin/" .. outputDir .. "/%{prj.name}")
    objdir("bin-int/" .. outputDir .. "/%{prj.name}")

    files
    {
        "Brickview/vendors/spdlog/src/**.cpp",
        "Brickview/vendors/spdlog/include/**.h"
    }

    includedirs
    {
        "Brickview/vendors/spdlog/include"
    }

    defines "SPDLOG_COMPILED_LIB"

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
        "Brickview/vendors/GLFW/include",
        "Brickview/vendors/GLEW/include",
        "Brickview/vendors/spdlog/include"
    }

    files
    {
        "Brickview/Brickview/src/**.h",
        "Brickview/Brickview/src/**.cpp"
    }

    links
    {
        "Brickview/vendors/GLFW/lib-vc2022/glfw3.lib",
        "Brickview/vendors/GLEW/lib/Release/Win32/glew32s.lib",
        "opengl32.lib",
        "spdlog"
    }

    defines
    {
        "GLEW_STATIC"
    }

    filter "configurations:Debug"
        defines "BV_DEBUG"

    filter "configurations:Release"
        defines "BV_RELEASE"
        optimize "on"