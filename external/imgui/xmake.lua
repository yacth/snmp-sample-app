add_rules("mode.debug", "mode.release")

add_requires("glfw")
add_requires("glad")

target("ImGui")
    set_kind("static")
    add_packages("glfw")
    add_packages("glad")
    
    -- Include ImGui headers
    add_includedirs("$(projectdir)/external/imgui")
    add_includedirs("$(projectdir)/external/imgui/backends")
    
    -- Add ImGui source files
    add_files("$(projectdir)/external/imgui/*.cpp")
    add_files("$(projectdir)/external/imgui/backends/imgui_impl_opengl3.cpp")
    add_files("$(projectdir)/external/imgui/backends/imgui_impl_glfw.cpp")
    set_targetdir("$(projectdir)/external/imgui/build")
