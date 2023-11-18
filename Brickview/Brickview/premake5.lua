project "Brickview"
    kind "ConsoleApp"
    language "C++"
    cppdialect "C++17"
    systemversion "latest"

    targetdir("bin/" .. outputDir .. "/%{prj.name}")
    objdir("bin-int/" .. outputDir .. "/%{prj.name}")

    includedirs
    {
        "src",
        "../vendors/GLFW/glfw-3.3.8/include",
        "../vendors/glad/include",
        "../vendors/spdlog/include"
    }

    files
    {
        "src/**.h",
        "src/**.cpp"
    }

    pchheader "Pch.h"
    pchsource "src/Pch.cpp"

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