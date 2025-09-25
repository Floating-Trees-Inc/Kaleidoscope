--
-- > Notice: Amélie Heinrich @ 2025
-- > Create Time: 2025-06-05 22:27:27
--

target("glm")
    set_kind("static")
    set_group("Third Party")

    add_files("stub.c")
    add_headerfiles("Include/**.hpp")
    add_includedirs("Include/", { public = true })
    add_defines("GLM_ENABLE_EXPERIMENTAL", { public = true })
