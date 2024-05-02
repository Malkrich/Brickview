project "BrickviewApp"
    kind "ConsoleApp"
    language "C++"
    cppdialect "C++17"
    staticruntime "off"

    targetdir("bin/" .. outputDir .. "/%{prj.name}")
    objdir("bin-int/" .. outputDir .. "/%{prj.name}")
    
    includedirs
    {
        "src",
        "../Brickview/src",
        "../vendors/glm/",
        "../vendors/imgui/",
        "../vendors/spdlog/include"
    }

    files
    {
        "src/**.h",
        "src/**.cpp"
    }

    links
    {
        "Brickview"
    }

    filter "system:windows"
        systemversion "latest"

    filter "configurations:Debug"
        defines "BV_DEBUG"
        runtime "Debug"

    filter "configurations:Release"
        defines "BV_RELEASE"
        runtime "Release"
        optimize "on"