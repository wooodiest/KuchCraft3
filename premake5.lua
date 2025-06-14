workspace "KuchCraft3"
    startproject "KuchCraft"
    outputdir = "%{cfg.buildcfg}-%{cfg.system}-%{cfg.architecture}"
    architecture "x64"
    configurations 
    { 
        "Debug",
        "Release",
        "Dist"
    }
    flags
    { 
        "MultiProcessorCompile" 
    }

project "KuchCraft"
    kind       "ConsoleApp"
    language   "C++"
    cppdialect "C++20"
    location   "KuchCraft"
    targetdir ("%{wks.location}/bin/"     .. outputdir .. "/%{prj.name}")
    objdir    ("%{wks.location}/bin-int/" .. outputdir .. "/%{prj.name}")

    pchheader "kcpch.h"
    pchsource "%{wks.location}/KuchCraft/src/kcpch.cpp"

    files
    {
        "%{wks.location}/KuchCraft/src/**.h",
        "%{wks.location}/KuchCraft/src/**.cpp"
    }

    includedirs
    {
        "%{wks.location}/KuchCraft/src",
        "%{wks.location}/KuchCraft/vendor",
        "%{wks.location}/KuchCraft/vendor/spdlog/include"
    }

    filter "system:windows"
        systemversion "latest"
        defines { "KC_PLATFORM_WINDOWS" }

    defines
    {
        "_CRT_SECURE_NO_WARNINGS"
    }

    filter "configurations:Debug or configurations:Release"
        defines { "KC_HAS_CONSOLE" }

    filter   "configurations:Debug"
        defines  "KC_DEBUG"
        runtime  "Debug"
        optimize "off"
        symbols  "on"

    filter   "configurations:Release"
        defines  "KC_RELEASE"
        runtime  "Release"
        optimize "on"
        symbols  "on"
        vectorextensions "AVX2"
		isaextensions { "BMI", "POPCNT", "LZCNT", "F16C" }

    filter   "configurations:Dist"
        defines  "KC_DIST"
        runtime  "Release"
        optimize "on"
        symbols  "off"
        vectorextensions "AVX2"
		isaextensions { "BMI", "POPCNT", "LZCNT", "F16C" }
