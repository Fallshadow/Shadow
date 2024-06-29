project "Editor"
	kind "ConsoleApp"
	language "C++"
	cppdialect "C++17"
	staticruntime "off"

	targetdir ("%{wks.location}/bin/" .. outputdir .. "/%{prj.name}")
	objdir ("%{wks.location}/bin-int/" .. outputdir .. "/%{prj.name}")

	files
	{
		"src/**.h",
		"src/**.cpp"
	}

	includedirs
	{
		"%{wks.location}/Shadow/src",
		"%{IncludeDir.spdlog}",
		"%{IncludeDir.glm}",
		"%{IncludeDir.Imgui}",
	}

	links
	{
		"Shadow"
	}

	filter "system:windows"
		systemversion "latest"

	filter "configurations:Debug"
		defines "SD_DEBUG"
		runtime "Debug"
		symbols "on"

	filter "configurations:Release"
		defines "SD_RELEASE"
		runtime "Release"
		optimize "on"