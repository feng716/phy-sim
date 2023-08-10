add_rules("mode.debug","mode.release")
add_rules("plugin.compile_commands.autoupdate",{outputdir="build"})
target("main")
    set_kind("binary")
    add_files("src/source/*.cpp","src/source/*.c")
    if is_mode("debug") then
        add_defines("DEBUG")
        add_cxflags("-g")
    end
    add_links("glfw","assimp","GL","imgui","spdlog","fmt")
    add_cxflags("-Wall")
    add_cxflags("-iquote./src/headers")
    add_cxflags("-xc++")
    after_build(function (target) 
        os.cp("3dmodels",target:targetdir())
    end)

    