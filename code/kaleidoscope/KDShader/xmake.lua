--
-- > Notice: Floating Trees Inc. @ 2025
-- > Create Time: 2025-07-15 23:02:48
--

target("KDShader")
    set_kind("static")
    set_group("Kaleidoscope")

    add_headerfiles("*.h", "Dummy/*.h")
    add_files("*.cpp", "Dummy/*.cpp")
    add_includedirs(".")

    add_deps("KernelOS", "KernelCore", "glm")

    if is_plat("windows") then
        add_headerfiles("DXIL/*.h", "SPIRV/*.h")
        add_files("DXIL/*.cpp", "SPIRV/*.cpp")
        add_deps("dxc", "spirv-reflect")

        add_syslinks("kernel32", "user32")
    elseif is_plat("macosx") then
        add_headerfiles("MetalLib/*.h")
        add_files("MetalLib/*.mm")
        add_deps("dxc", "metalirconverter")
    end