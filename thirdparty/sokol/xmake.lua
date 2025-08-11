--
-- > Notice: Floating Trees Inc. @ 2025
-- > Create Time: 2025-08-11 21:14:13
--

target("sokol")
    set_kind("static")
    set_group("Third Party")

    add_headerfiles("*.h")
    add_files("*.c")
    add_includedirs(".", { public = true })
