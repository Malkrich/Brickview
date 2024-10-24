project "Brickview"
    kind "StaticLib"
    language "C++"
    cppdialect "C++20"
    staticruntime "off"

    targetdir("bin/" .. outputDir .. "/%{prj.name}")
    objdir("bin-int/" .. outputDir .. "/%{prj.name}")

    includedirs
    {
        "src/",
        "src/Brickview/",
        "src/Vendors/",
        "../Vendors/glm/",
        "../Vendors/GLFW/glfw-3.3.8/include",
        "../Vendors/glad/include",
        "../Vendors/spdlog/include",
        "../Vendors/imgui/",
        "../Vendors/entt/include/",
        "../Vendors/stb_image/"
    }

    files
    {
        "src/**.h",
        "src/**.cpp",

        -- glm
        "../vendors/glm/glm/**.hpp",
        "../vendors/glm/glm/**.inl",
        -- stb image
        "../vendors/stb_image/**.h",
        "../vendors/stb_image/**.cpp"
    }

    pchheader "Pch.h"
    pchsource "src/Pch.cpp"

    links
    {
        "GLFW",
        "glad",
        "spdlog",
        "imgui",
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