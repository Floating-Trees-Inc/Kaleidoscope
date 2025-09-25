--
-- > Notice: Floating Trees Inc. @ 2025
-- > Create Time: 2025-07-26 00:12:25
--

target("flip")
    set_kind("static")
    set_group("Third Party")

    add_files("stub.c")
    add_headerfiles("flip.h")
    add_includedirs(".", { public = true })
