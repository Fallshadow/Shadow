workspace "Shadow"
	architecture("x86_64")
	startproject "Editor"

    configurations
	{
		"Debug",
		"Release",
	}

	outputdir = "%{cfg.buildcfg}-%{cfg.system}-%{cfg.architecture}"

	IncludeDir = {}
	IncludeDir["spdlog"] = "%{wks.location}/Shadow/vendor/spdlog/include"
	IncludeDir["glfw"] = "%{wks.location}/Shadow/vendor/glfw/include"



group "Core"
	include "Shadow"
group ""

group "Editor"
	include "Editor"
group ""

group "Dependencies"
	include "vendor/premake"
	include "Shadow/vendor/glfw"
group ""