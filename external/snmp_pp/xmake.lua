add_rules("mode.debug", "mode.release")

add_requires("openssl")

package("snmp++-3.5.2")
    add_deps("cmake")
    add_deps("openssl", { configs = { shared = true } })
    if is_os("windows") then
        -- For Windows, link against system libraries
        add_syslinks("ws2_32", "crypt32")
    else
        -- For other platforms (like Linux), link against specific libraries
        add_links("ssl", "crypto")
    end

    set_sourcedir("$(projectdir)/external/snmp_pp")
    on_install(function (package)
        local openssl = package:dep("openssl")
        local configs = {}
        table.insert(configs, "-DCMAKE_BUILD_TYPE=" .. (package:debug() and "Debug" or "Release"))
        table.insert(configs, "-DBUILD_SHARED_LIBS=" .. (package:config("static") and "ON" or "OFF"))
        table.insert(configs, "-DOPENSSL_ROOT_DIR=" .. openssl:installdir())

        -- Set the build directory
        local build_dir = "$(projectdir)/external/snmp_pp/build"
        os.mkdir(build_dir)
        import("package.tools.cmake").install(package, configs, {buildir = build_dir})
    end)
package_end()

add_requires("snmp++-3.5.2")

target("SNMP++")
    set_kind("static")
    add_packages("snmp++-3.5.2")
    set_targetdir("$(projectdir)/external/snmp_pp/build")