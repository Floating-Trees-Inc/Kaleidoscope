--
-- > Notice: Floating Trees Inc. @ 2025
-- > Create Time: 2025-07-05 13:10:14
--

add_rules("mode.debug", "mode.release", "mode.releasedbg")

set_rundir(".")
set_languages("c++20", { public = true })

-- Platform
if is_plat("windows") then
    add_defines("KD_WINDOWS", "USE_PIX", { public = true })
elseif is_plat("linux") then
    add_defines("KD_LINUX", { public = true })
elseif is_plat("macosx") then
    add_defines("KD_MAC", { public = true })
    add_rpathdirs("dlls/mac")
end

-- Config
add_defines("GLM_FORCE_DEPTH_ZERO_TO_ONE", { public = true })
if is_mode("debug") then
    add_defines("KD_DEBUG", { public = true })
    set_symbols("debug", { public = true })
    set_optimize("none", { public = true })
elseif is_mode("releasedbg") then
    add_defines("KD_RELEASE", { public = true })
    set_symbols("debug", { public = true })
    set_optimize("fastest", { public = true })
    set_strip("all", { public = true })
else
    add_defines("KD_RETAIL", { public = true })
    set_symbols("hidden", { public = true })
    set_optimize("fastest", { public = true })
    set_strip("all", { public = true })
end

-- Includes
includes("thirdparty")
includes("code/kaleidoscope")
includes("code/samples")

-- Post build step
before_run(function (target)
    if is_plat("windows") then
        os.cp("dlls/windows/*", "$(builddir)/$(plat)/$(arch)/$(mode)/")
    end
end)
