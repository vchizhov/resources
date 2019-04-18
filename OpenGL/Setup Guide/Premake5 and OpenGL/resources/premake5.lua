workspace "GettingStartedWithOpenGL"
	startproject "Sandbox"

    configurations {
        "Debug",
        "Release"
    }

    platforms {
        "x86",
        "x64"
    }

    IncludeDir = {}
    IncludeDir["glad"] = "%{sln.location}/dependencies/glad/include"
    IncludeDir["glfw"] = "%{sln.location}/dependencies/glfw/include"
    IncludeDir["qtl"] = "%{sln.location}/dependencies/qtl/include"

    sln = solution();
    outputdir = "%{cfg.buildcfg}/%{cfg.system}/%{cfg.architecture}"

project "glad"
    kind "StaticLib"
    language "C++"

    targetdir ("%{sln.location}/" .. "bin/" .. outputdir .. "/%{prj.name}")
    objdir ("%{sln.location}/" .. "bin-int/" .. outputdir .. "/%{prj.name}")
    targetname("%{prj.name}")

    files {
        "**.h",
        "**.c",
        "**.hpp",
        "**.cpp"
    }

    includedirs {
        "%{IncludeDir.glad}"
    }

    filter "system:windows"
        cppdialect "C++17"
        staticruntime "Off"
        systemversion "latest"
        toolset "v142"

    filter "system:linux"
        buildoptions "-std=c++17"
        staticruntime "Off"

    filter {}

    filter "configurations:Debug"
        runtime "Debug"
        symbols "On"

    filter "configurations:Release"
        runtime "Release"
        symbols "Off"
        optimize "Full"
    
    filter {}

project "glfw"
	kind "StaticLib"
    language "C++"

    targetdir ("%{sln.location}/" .. "bin/" .. outputdir .. "/%{prj.name}")
    objdir ("%{sln.location}/" .. "bin-int/" .. outputdir .. "/%{prj.name}")
    targetname("%{prj.name}")
    
    filter {}
	
	includedirs {
        "include",
    }

    files {
        "src/context.c",
        "src/init.c",
        "src/input.c",
        "src/monitor.c",
        "src/vulkan.c",
        "src/window.c"
    }

	filter "system:windows"
        systemversion "latest"
        toolset "v142"
        cppdialect "C++17"
        staticruntime "Off"

        defines
        {
            "_CRT_SECURE_NO_WARNINGS",
            "_GLFW_WIN32"
        }

        files {
            "src/win32_platform.h",
            "src/win32_joystick.h",
            "src/wgl_context.h",
            "src/egl_context.h",
            "src/win32_init.c",
            "src/win32_joystick.c",
            "src/win32_monitor.c",
            "src/win32_time.c",
            "src/win32_tls.c",
            "src/win32_window.c",
            "src/wgl_context.c",
            "src/egl_context.c"
        }

	filter "system:linux"
		buildoptions "-std=c++17"
		buildoptions "-Wall"
        linkoptions "-pthread"
        buildoptions "-ggdb3"

        defines {
            "_GLFW_X11"
        }

        files {
            "src/x11_platform.h",
            "src/xkb_unicode.h",
            "src/linux_joystick.h",
            "src/posix_time.h",
            "src/posix_tls.h",
            "src/glx_context.h",
            "src/egl_context.h",
            "src/x11_init.c",
            "src/x11_monitor.c",
            "src/x11_window.c",
            "src/xkb_unicode.c",
            "src/linux_joystick.c",
            "src/posix_time.c",
            "src/posix_tls.c",
            "src/glx_context.c",
            "src/egl_context.c"
        }

    filter {}
	
	filter "configurations:Debug"
        runtime "Debug"
        symbols "On"

		defines 
		{ 
			"_DEBUG"
        }
	
	filter "configurations:Release"
		runtime "Release"
        optimize "Speed"

    filter {}

project "Sandbox"
    kind "ConsoleApp"
    language "C++"

    targetdir ("%{sln.location}/bin/" .. outputdir .. "/%{prj.name}")
    objdir ("%{sln.location}/bin-int/" .. outputdir .. "/%{prj.name}")

    files {
        "**.h",
        "**.c",
        "**.hpp",
        "**.cpp"
    }

    includedirs {
        "%{IncludeDir.glad}",
        "%{IncludeDir.glfw}",
        "%{IncludeDir.sandbox}"
    }

    dependson {
        "glad",
        "glfw"
    }

    libdirs {
        "%{sln.location}/bin/" .. outputdir .. "/glad",
        "%{sln.location}/bin/" .. outputdir .. "/glfw"

    }

    links {
        "glad",
        "glfw"
    }

    filter "system:windows"
        cppdialect "C++17"
        staticruntime "Off"
        systemversion "latest"
        toolset "v142"

    filter "system:linux"
        buildoptions "-std=c++17"
        staticruntime "Off"

    filter {}

    filter "configurations:Debug"
        runtime "Debug"
        symbols "On"

    filter "configurations:Release"
        runtime "Release"
        symbols "Off"
        optimize "Full"
    
    filter {}