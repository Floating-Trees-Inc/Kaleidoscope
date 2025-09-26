--
-- > Notice: Amélie Heinrich @ 2025
-- > Create Time: 2025-09-26 18:35:26
--

target("metalirconverter")
    set_kind("headeronly")
    set_group("Third Party")
    
    if is_plat("macosx") then
        set_kind("static")
        add_files("Source/runtime.mm")
        add_headerfiles("Include/**.h")
        add_includedirs("Include/", { public = true })
        add_linkdirs("Libs/")
        add_links("metalirconverter")
        add_frameworks("Metal", "Foundation")
    end
