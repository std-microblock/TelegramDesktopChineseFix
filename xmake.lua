set_project("telegram-desktop-chinese-fix")
set_policy("compatibility.version", "3.0")

set_languages("c++23")
set_warnings("all") 
add_rules("plugin.compile_commands.autoupdate", {outputdir = "build"})
add_rules("mode.debug", "mode.release")

package("blook")
    add_deps("cmake")
    add_syslinks("advapi32")
    set_sourcedir(path.join(os.scriptdir(), "blook"))
    on_install(function (package)
        local fcdir = package:cachedir() .. "/fetchcontent"
        import("package.tools.cmake").install(package, {
                "-DCMAKE_INSTALL_PREFIX=" .. package:installdir(),
                "-DCMAKE_PREFIX_PATH=" .. package:installdir(),
                "-DFETCHCONTENT_QUIET=OFF",
                "-DFETCHCONTENT_BASE_DIR=" .. fcdir,
        })
        
        os.cp("include/blook/**", package:installdir("include/blook/"))
        os.cp("external/zasm/zasm/include/**", package:installdir("include/zasm/"))
        os.cp(fcdir .. "/zydis-src/dependencies/zycore/include/**", package:installdir("include/zycore/"))
        os.cp(package:buildir() .. "/blook.lib", package:installdir("lib"))
        os.cp(package:buildir() .. "/external/zasm/zasm.lib", package:installdir("lib"))
    end)
package_end()
add_requires("blook")

target("telegram-desktop-chinese-fix")
    set_kind("binary")
    -- windows subsystem
    -- add_ldflags("-subsystem:windows")
    add_packages("blook")
    add_files("src/main.cc")
