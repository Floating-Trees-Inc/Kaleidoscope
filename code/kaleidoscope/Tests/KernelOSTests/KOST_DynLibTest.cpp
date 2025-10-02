//
// > Notice: Floating Trees Inc. @ 2025
// > Create Time: 2025-07-16 19:36:30
//

#include <KernelOS/KOS_DynLib.h>
#include <catch2/catch_test_macros.hpp>

static const char* kOpenGLLibPath() {
#if defined(KD_WINDOWS)
    return "opengl32.dll";
#elif defined(KD_MAC)
    // System OpenGL framework (deprecated but present on macOS).
    // Either of these works; the first is cleaner:
    return "/System/Library/Frameworks/OpenGL.framework/OpenGL";
#else
#   error "Define KD_WINDOWS or KD_MAC for this test."
#endif
}

TEST_CASE("DynLib opens OpenGL", "[dynlib][opengl]") {
    KOS::DynLib lib(kOpenGLLibPath());
    REQUIRE(lib.Valid());
}

TEST_CASE("DynLib finds glDrawArrays", "[dynlib][opengl]") {
    KOS::DynLib lib(kOpenGLLibPath());
    REQUIRE(lib.Valid());

    // glDrawArrays is in core OpenGL on both platforms.
    auto fn = lib.GetFunction("glDrawArrays");
    REQUIRE(fn != nullptr);
}
