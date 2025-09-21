--
-- > Notice: Floating Trees Inc. @ 2025
-- > Create Time: 2025-07-25 21:04:47
--

target("vma")
    set_group("Third Party")
    add_deps("volk")

    if is_plat("windows") or is_plat("linux") then
        set_kind("static")
        add_files("*.cpp")
        add_headerfiles("*.h")
        add_includedirs(".", { public = true })
    else
        set_kind("headeronly")
    end
