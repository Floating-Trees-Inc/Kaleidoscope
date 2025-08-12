--
-- > Notice: Floating Trees Inc. @ 2025
-- > Create Time: 2025-08-12 10:40:00
--

target("im3d")
    set_kind("static")
    set_group("Third Party")

    add_files("*.cpp")
    add_headerfiles("*.h")
    add_includedirs(".", { public = true })
