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

if os.target() == "windows" then
   include ("ComputerGraphicsProject2023/vendor/GLFW/")
end

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
        "%{prj.name}/vendor/glm/glm/**.hpp",
        "%{prj.name}/src/**.cpp"
    }

    includedirs
    {
        "%{prj.name}/src",
        "%{prj.name}/vendor/glm",
        "%{prj.name}/vendor/stb",
        "%{prj.name}/vendor/tinyobj"
    }
    
    postbuildcommands {
        "glslc %{prj.location}/res/shaders/base.vert -o %{prj.location}/res/shaders/baseVert.spv",
        "glslc %{prj.location}/res/shaders/base.frag -o %{prj.location}/res/shaders/baseFrag.spv",
        "glslc %{prj.location}/res/shaders/UItextSDF.vert -o %{prj.location}/res/shaders/UItextSDF_vert.spv",
        "glslc %{prj.location}/res/shaders/UItextSDF.frag -o %{prj.location}/res/shaders/UItextSDF_frag.spv",
        "{COPYDIR} %{prj.location}/res %{cfg.targetdir}/res"
    }

    filter "system:windows"
        systemversion "latest"
        
        includedirs {
            "%{IncludeDirs.GLFW}",
            "%{IncludeDirs.VulkanSDK}"
        }
        
        links
        { 
            "GLFW",
            "%{Library.Vulkan}"
        }
        
        -- defines {  }

    filter "system:linux"
        systemversion "latest"
        
        links {
            "vulkan",
            "glfw"
        }
    
    filter "configurations:Debug"
        defines "DEBUG"
        symbols "On"

    filter "configurations:Release"
        defines "NDEBUG"
        optimize "On"
