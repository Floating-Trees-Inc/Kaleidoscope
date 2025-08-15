--
-- > Notice: Floating Trees Inc. @ 2025
-- > Create Time: 2025-08-15 19:03:16
--

target("ToolDevConsole")
    set_kind("static")
    set_group("Kaleidoscope")

    add_headerfiles("*.h")
    add_includedirs(".")
    add_files("*.cpp")

    add_deps("imgui", { public = true })
    add_deps("KernelOS", "KernelCore", "KernelInput")
