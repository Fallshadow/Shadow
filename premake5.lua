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
	IncludeDir["glad"] = "%{wks.location}/Shadow/vendor/glad/include"
	IncludeDir["glm"] = "%{wks.location}/Shadow/vendor/glm"
	IncludeDir["stb_image"] = "%{wks.location}/Shadow/vendor/stb_image"
	IncludeDir["Imgui"] = "%{wks.location}/Shadow/vendor/imgui"
	IncludeDir["yaml_cpp"] = "%{wks.location}/Shadow/vendor/yaml-cpp/include"
	IncludeDir["entt"] = "%{wks.location}/Shadow/vendor/entt/include"
	IncludeDir["ImGuizmo"] = "%{wks.location}/Shadow/vendor/ImGuizmo"
	IncludeDir["msdfgen"] = "%{wks.location}/Shadow/vendor/msdf-atlas-gen/msdfgen"
	IncludeDir["msdf_atlas_gen"] = "%{wks.location}/Shadow/vendor/msdf-atlas-gen/msdf-atlas-gen"
	IncludeDir["Box2D"] = "%{wks.location}/Shadow/vendor/Box2D/include"
	IncludeDir["mono"] = "%{wks.location}/Shadow/vendor/mono/include"

	LibraryDir = {}
	LibraryDir["mono"] = "%{wks.location}/Hazel/vendor/mono/lib/%{cfg.buildcfg}"

	Library = {}
	Library["mono"] = "%{LibraryDir.mono}/libmono-static-sgen.lib"
group "Core"
	include "Shadow"
group ""

group "Editor"
	include "Editor"
group ""

group "Dependencies"
	include "vendor/premake"
	include "Shadow/vendor/glfw"
	include "Shadow/vendor/glad"
	include "Shadow/vendor/imgui"
	include "Shadow/vendor/yaml-cpp"
	include "Shadow/vendor/msdf-atlas-gen"
	include "Shadow/vendor/Box2D"
group ""