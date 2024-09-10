add_rules("mode.debug", "mode.release")

includes("external/snmp_pp/xmake.lua")

target("Sensuron")
  set_kind("binary")

  add_deps("SNMP++")

  add_includedirs("$(projectdir)/external/snmp_pp")
  add_includedirs("$(projectdir)/external/snmp_pp/include")
  add_includedirs("$(projectdir)/sample-app/sensuron_agent/")

  add_files("$(projectdir)/sample-app/sensuron_agent/*.cpp")
  add_files("$(projectdir)/sample-app/src/*.cpp")

  add_linkdirs("$(projectdir)/external/snmp_pp/build")
  add_links("SNMP++")