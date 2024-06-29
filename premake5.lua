workspace "Shadow"
	architecture("x86_64")
	startproject "Editor"

    configurations
	{
		"Debug",
		"Release",
	}

	flags
	{
		"MultiProcessorCompile"
	}

	outputdir = "%{cfg.buildcfg}-%{cfg.system}-%{cfg.architecture}"

	IncludeDir = {}
	IncludeDir["spdlog"] = "%{wks.location}/Shadow/vendor/spdlog/include"
	IncludeDir["glfw"] = "%{wks.location}/Shadow/vendor/glfw/include"
	IncludeDir["glm"] = "%{wks.location}/Shadow/vendor/glm"



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