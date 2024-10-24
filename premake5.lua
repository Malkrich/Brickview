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
    include "BrickviewApp"

group "Core"
    include "Brickview/Brickview"

group "Dependencies"
    include "Brickview/Vendors/GLFW"
    include "Brickview/Vendors/glad"
    include "Brickview/Vendors/spdlog"
    include "Brickview/Vendors/imgui"