--
-- > Notice: Floating Trees Inc. @ 2025
-- > Create Time: 2025-07-25 20:20:19
--

target("volk")
    set_group("Third Party")

    if is_plat("windows") or is_plat("linux") then
        set_kind("static")

        add_headerfiles("*.h")
        add_files("*.cpp")
        
        add_includedirs("include", ".", { public = true })
        add_defines("VK_NO_PROTOTYPES", { public = true })
    else
        set_kind("headeronly")
    end
