--
-- > Notice: Floating Trees Inc. @ 2025
-- > Create Time: 2025-10-04 15:37:00
--

target("ToolEditor")
    set_kind("binary")
    set_group("Kaleidoscope")

    add_files("**.cpp")
    add_headerfiles("**.h")
    add_includedirs(".")
    add_deps("Kaleidoscope", "imgui-node-editor")
