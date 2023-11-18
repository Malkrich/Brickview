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

group "Application"
    include "Brickview/Brickview"
        
group "Dependencies"
    include "Brickview/vendors/GLFW"
    include "Brickview/vendors/glad"
    include "Brickview/vendors/spdlog"