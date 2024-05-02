workspace "Brickview"
    architecture "x86"
    startproject "BrickviewApp"
    
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

group "Application"
    include "Brickview/BrickviewApp"

group "Core"
    include "Brickview/Brickview"

group "Dependencies"
    include "Brickview/vendors/GLFW"
    include "Brickview/vendors/glad"
    include "Brickview/vendors/spdlog"
    include "Brickview/vendors/imgui"