workspace "ComputerGraphicsProject2023"
    architecture "x64"
    configurations { "Debug", "Release" }

VULKAN_SDK = os.getenv("VULKAN_SDK")

outputdir = "%{cfg.buildcfg}-%{cfg.system}-%{cfg.architecture}"

IncludeDirs = {}
IncludeDirs["VulkanSDK"] = "%{VULKAN_SDK}/Include"
IncludeDirs["GLFW"] = "ComputerGraphicsProject2023/vendor/GLFW/include"

LibraryDir = {}
LibraryDir["VulkanSDK"] = "%{VULKAN_SDK}/Lib"

Library = {}
Library["Vulkan"] = "%{LibraryDir.VulkanSDK}/vulkan-1.lib"

include ("ComputerGraphicsProject2023/vendor/GLFW/")

project "ComputerGraphicsProject2023"
    location "ComputerGraphicsProject2023"
    kind "ConsoleApp"
    language "C++"
    cppdialect "C++17"
    staticruntime "On"

    targetdir ("bin/" .. outputdir .. "/%{prj.name}")
    objdir ("bin-int/" .. outputdir .. "/%{prj.name}")

    files
    { 
        "%{prj.name}/src/**.h",
        "%{prj.name}/src/**.cpp"
    }

    includedirs
    {
        "%{prj.name}/vendor/glm/glm",
        "%{prj.name}/vendor/stb",
        "%{IncludeDirs.GLFW}",
        "%{IncludeDirs.VulkanSDK}"
    }

    links
    { 
        "GLFW",
        "%{Library.Vulkan}"
    }

    filter "system:windows"
        systemversion "latest"

        -- defines {  }

    filter "system:linux"
        systemversion "latest"
    
    filter "configurations:Debug"
        defines "DEBUG"
        symbols "On"

    filter "configurations:Release"
        defines "NDEBUG"
        optimize "On"
