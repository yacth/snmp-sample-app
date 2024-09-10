add_rules("mode.debug", "mode.release")


includes("external/imgui/xmake.lua")
includes("external/snmp_pp/xmake.lua")

target("Sensuron")
  set_kind("binary")
  add_deps("SNMP++")
  add_deps("ImGui")
  add_packages("glfw")
  add_packages("glad")

  add_includedirs("$(projectdir)/external/")
  add_includedirs("$(projectdir)/external/snmp_pp")
  add_includedirs("$(projectdir)/external/snmp_pp/include")
  add_includedirs("$(projectdir)/external/imgui/")
  add_includedirs("$(projectdir)/sample-app/sensuron_agent/")

  
  add_files("$(projectdir)/sample-app/sensuron_agent/*.cpp")
  add_files("$(projectdir)/sample-app/src/*.cpp")

  add_linkdirs("$(projectdir)/external/snmp_pp/build")
  add_links("SNMP++")

  add_linkdirs("$(projectdir)/external/imgui/build")
  add_links("ImGui")


  after_build(function (target)
    local target_font_folder_path = target:targetdir() .. "/fonts"
    if not os.isdir(target_font_folder_path) then
      os.mkdir(target_font_folder_path)
      os.cp("$(projectdir)/external/imgui/misc/fonts/*.ttf", target_font_folder_path)
    end
  end)
