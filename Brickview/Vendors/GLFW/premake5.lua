project "GLFW"
    kind "StaticLib"
    language "C"

    targetdir("bin/" .. outputDir .. "/%{prj.name}")
    objdir("bin-int/" .. outputDir .. "/%{prj.name}")

    files
    {
		"glfw-3.3.8/include/GLFW/glfw3.h",
		"glfw-3.3.8/include/GLFW/glfw3native.h",
		"glfw-3.3.8/src/glfw_config.h",
		"glfw-3.3.8/src/context.c",
		"glfw-3.3.8/src/init.c",
		"glfw-3.3.8/src/input.c",
		"glfw-3.3.8/src/monitor.c",
		"glfw-3.3.8/src/vulkan.c",
		"glfw-3.3.8/src/window.c"
    }

    filter "system:windows"
        systemversion "latest"
    
        files
        {
            "glfw-3.3.8/src/win32_init.c",
			"glfw-3.3.8/src/win32_joystick.c",
			"glfw-3.3.8/src/win32_monitor.c",
			"glfw-3.3.8/src/win32_time.c",
			"glfw-3.3.8/src/win32_thread.c",
			"glfw-3.3.8/src/win32_window.c",
			"glfw-3.3.8/src/wgl_context.c",
			"glfw-3.3.8/src/egl_context.c",
			"glfw-3.3.8/src/osmesa_context.c"
        }

        defines
        {
            "_GLFW_WIN32",
			"_CRT_SECURE_NO_WARNINGS"
        }

    filter "configurations:Debug"
        runtime "Debug"
        symbols "on"

    filter "configurations:Release"
        runtime "Release"
        optimize "on"