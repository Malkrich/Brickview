project "BrickviewApp"
    kind "ConsoleApp"
    language "C++"
    cppdialect "C++20"
    staticruntime "off"

    targetdir("bin/" .. outputDir .. "/%{prj.name}")
    objdir("bin-int/" .. outputDir .. "/%{prj.name}")

    includedirs
    {
        "src/",
        "../Brickview/Brickview/src/Brickview/",
        "../Brickview/Vendors/glm/",
        "../Brickview/Vendors/imgui/",
        "../Brickview/Vendors/spdlog/include",
        "../Brickview/Vendors/entt/include/"
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