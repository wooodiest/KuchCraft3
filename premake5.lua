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

group "Dependencies"
	include "KuchCraft/vendor/glfw"
    include "KuchCraft/vendor/glad"
    include "KuchCraft/vendor/imgui"
group ""

project "KuchCraft"
    kind       "WindowedApp"
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
        "%{wks.location}/KuchCraft/src/**.cpp",
        "%{wks.location}/KuchCraft/vendor/glm/glm/**.hpp",
        "%{wks.location}/KuchCraft/vendor/glm/glm/**.inl",
        "%{wks.location}/KuchCraft/vendor/stb_image/**.h",
		"%{wks.location}/KuchCraft/vendor/stb_image/**.cpp",
        "%{wks.location}/KuchCraft/vendor/magic_enum/**.hpp",

        "%{wks.location}/KuchCraft/vendor/imgui/backends/imgui_impl_glfw.cpp",
        "%{wks.location}/KuchCraft/vendor/imgui/backends/imgui_impl_opengl3.cpp",
        "%{wks.location}/KuchCraft/vendor/imgui/misc/cpp/imgui_stdlib.h",
        "%{wks.location}/KuchCraft/vendor/imgui/misc/cpp/imgui_stdlib.cpp"
    }

    includedirs
    {
        "%{wks.location}/KuchCraft/src",
        "%{wks.location}/KuchCraft/vendor",
        "%{wks.location}/KuchCraft/vendor/glfw/include",
        "%{wks.location}/KuchCraft/vendor/glad/include",
        "%{wks.location}/KuchCraft/vendor/spdlog/include",
        "%{wks.location}/KuchCraft/vendor/glm",
        "%{wks.location}/KuchCraft/vendor/stb_image",
        "%{wks.location}/KuchCraft/vendor/magic_enum",
        "%{wks.location}/KuchCraft/vendor/imgui",
        "%{wks.location}/KuchCraft/vendor/entt/src",
        "%{wks.location}/KuchCraft/vendor/nlohmann_json",
    }

    links
    {
        "GLFW",
        "Glad",
        "opengl32.lib",
        "ImGui"
    }

    defines
    {
        "_CRT_SECURE_NO_WARNINGS",
        "GLFW_INCLUDE_NONE"
    }

    filter "files:**/imgui_stdlib.cpp"
        flags { "NoPCH" }
    filter "files:**/imgui_impl_glfw.cpp"
        flags { "NoPCH" }
    filter "files:**/imgui_impl_opengl3.cpp"
        flags { "NoPCH" }

    filter "system:windows"
        systemversion "latest"
        defines { "KC_PLATFORM_WINDOWS" }

    filter "configurations:Debug or configurations:Release"
        defines { "KC_HAS_CONSOLE" }
        kind "ConsoleApp"

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
