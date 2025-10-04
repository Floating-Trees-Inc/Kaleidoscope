--
-- > Notice: Floating Trees Inc. @ 2025
-- > Create Time: 2025-10-04 15:26:17
--

target("WorldTests")
    set_kind("binary")
    set_group("Kaleidoscope")

    add_files("*.cpp")
    add_deps("KernelOS", "KernelCore", "World", "catch2")
    add_defines("KD_DISABLE_LOGGING")
