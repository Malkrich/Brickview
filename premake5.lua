workspace "Brickview"
    architecture "x86"

    configurations
    {
        "Debug",
        "Release"
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
        "Brickview/vendors/GLFW/include",
        "Brickview/vendors/GLEW/include"
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
        "opengl32.lib"
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