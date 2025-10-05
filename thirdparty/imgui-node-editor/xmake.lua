--
-- > Notice: Floating Trees Inc. @ 2025
-- > Create Time: 2025-10-05 22:24:44
--

target("imgui-node-editor")
    set_kind("static")
    set_group("Third Party")

    add_deps("imgui")
    add_headerfiles("include/*.h")
    add_files("include/*.cpp")
    add_includedirs("include", { public = true })
