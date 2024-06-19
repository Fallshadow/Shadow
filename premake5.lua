workspace "Shadow"
	architecture("x86_64")
	startproject "Editor"

    configurations
	{
		"Debug",
		"Release",
	}

	outputdir = "%{cfg.buildcfg}-%{cfg.system}-%{cfg.architecture}"

group "Core"
	include "Shadow"
group ""

group "Editor"
	include "Editor"
group ""

group "Dependencies"
	include "vendor/premake"
group ""