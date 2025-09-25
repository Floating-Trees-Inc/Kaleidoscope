--
-- > Notice: Floating Trees Inc. @ 2025
-- > Create Time: 2025-08-20 20:29:17
--

target("font-awesome")
    set_kind("static")
    set_group("Third Party")

    add_files("stub.c")
    add_headerfiles("*.h")
    add_includedirs(".", { public = true })
