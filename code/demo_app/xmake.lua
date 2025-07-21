--
-- > Notice: Floating Trees Inc. @ 2025
-- > Create Time: 2025-07-05 13:16:15
--

target("demo_app")
    set_kind("binary")
    set_languages("c++20")
    set_group("Games")

    add_files("*.cpp")
    add_deps("Kaleidoscope")
