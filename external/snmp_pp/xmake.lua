-- Define build modes
add_rules("mode.debug", "mode.release")

-- Add OpenSSL dependency
add_requires("openssl")

-- Define the SNMP++ package
package("snmp++-3.5.2")
    add_deps("cmake")
    add_deps("openssl", { configs = { shared = true } })

    -- Platform-specific linking
    if is_os("windows") then
        add_syslinks("ws2_32", "crypt32")
    else
        add_links("ssl", "crypto")
    end

    -- Specify the source directory
    set_sourcedir("$(projectdir)/external/snmp_pp")

    on_install(function (package)
        local openssl = package:dep("openssl")
        local configs = {}
        table.insert(configs, "-DCMAKE_BUILD_TYPE=" .. (package:debug() and "Debug" or "Release"))
        table.insert(configs, "-DBUILD_SHARED_LIBS=" .. (package:config("shared") and "ON" or "OFF"))
        table.insert(configs, "-DOPENSSL_ROOT_DIR=" .. openssl:installdir())

        -- Set the build directory
        local build_dir = "$(projectdir)/external/snmp_pp/build"
        os.mkdir(build_dir)
        import("package.tools.cmake").install(package, configs, {buildir = build_dir})
    end)
package_end()

-- Add the SNMP++ package as a requirement
add_requires("snmp++-3.5.2")

-- Define the SNMP++ target
target("SNMP++")
    set_kind("shared")
    add_packages("snmp++-3.5.2")

    -- Include source files and directories
    add_files("$(projectdir)/external/snmp_pp/src/*.cpp")
    add_includedirs("$(projectdir)/external/snmp_pp/include")

    -- Specify the target directory
    set_targetdir("$(projectdir)/external/snmp_pp/build")