project "SoLoud"
	kind "StaticLib"
	language "C++"

	targetdir ("bin/" .. outputdir .. "/%{prj.name}")
	objdir ("bin-int/" .. outputdir .. "/%{prj.name}")

	files
	{
        "include/**.h",
        "src/core/**.cpp",
        "src/audiosource/**.cpp",
        "src/audiosource/**.c",
        "src/c_api/**.cpp",
        "src/filter/**.cpp",
        "src/tool/**.cpp"
	}
    
    includedirs
    {
        "include",
    }
    
	filter "system:linux"
		pic "On"

		systemversion "latest"
		staticruntime "On"

        files
        {
            "src/backend/oss/**.cpp",
            "src/backend/alsa/**.cpp"
        }
        
        defines
        {
            "WITH_OSS",
            "WITH_ALSA"
        }

	filter "system:windows"
		systemversion "latest"
		staticruntime "On"

        files
        {
            "src/backend/winmm/**.cpp"
        }
        
        defines
        {
            "WITH_WINMM"
        }

	filter "configurations:Debug"
		runtime "Debug"
		symbols "on"

	filter "configurations:Release"
		runtime "Release"
		optimize "on"