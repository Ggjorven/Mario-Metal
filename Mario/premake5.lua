local MacOSVersion = MacOSVersion or "14.5"
local OutputDir = OutputDir or "%{cfg.buildcfg}-%{cfg.system}"

project "Mario"
	kind "ConsoleApp"
	language "C++"
	cppdialect "C++23"
	staticruntime "On"

	architecture "x86_64"

	warnings "Extra"

	targetdir ("%{wks.location}/bin/" .. OutputDir .. "/%{prj.name}")
	objdir ("%{wks.location}/bin-int/" .. OutputDir .. "/%{prj.name}")

	-- Note: VS2022/Make only need the pchheader filename
	pchheader "mmpch.h"
	pchsource "src/mmpch.cpp"

	files
	{
		"src/**.h",
		"src/**.hpp",
		"src/**.inl",
		"src/**.cpp"
	}

	defines
	{
		"_CRT_SECURE_NO_WARNINGS",
		"_SILENCE_ALL_MS_EXT_DEPRECATION_WARNINGS",

		"NANO_EXPERIMENTAL",
		"OB_API_VULKAN"
	}

	includedirs
	{
		"src",
		"src/Mario",
	}

	includedirs(Dependencies.Nano.IncludeDir)

	includedirs(Dependencies.Obsidian.IncludeDir)
	links(Dependencies.Obsidian.LibName)
	postbuildcommands(Dependencies.Obsidian.PostBuildCommands)

	includedirs(Dependencies.Photon.IncludeDir)
	links(Dependencies.Photon.LibName)
	postbuildcommands(Dependencies.Photon.PostBuildCommands)

	filter "system:windows"
		systemversion "latest"
		staticruntime "on"

        defines
        {
            "NOMINMAX"
        }

	filter "system:linux"
		systemversion "latest"
		staticruntime "on"

    filter "system:macosx"
		systemversion(MacOSVersion)
		staticruntime "on"

	filter "action:vs*"
		buildoptions { "/Zc:preprocessor" }

	filter "action:xcode*"
		-- Note: XCode only needs the full pchheader path
		pchheader "src/mmpch.h"

		-- Note: If we don't add the header files to the externalincludedirs
		-- we can't use <angled> brackets to include files.
		externalincludedirs(includedirs())

	filter "configurations:Debug"
		defines "MM_CONFIG_DEBUG"
		runtime "Debug"
		symbols "on"
		
	filter "configurations:Release"
		defines "MM_CONFIG_RELEASE"
		runtime "Release"
		optimize "on"

	filter "configurations:Dist"
		defines "MM_CONFIG_DIST"
		runtime "Release"
		optimize "Full"
		linktimeoptimization "on"
