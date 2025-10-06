//
// > Notice: Floating Trees Inc. @ 2025
// > Create Time: 2025-10-06 08:16:08
//

#include <catch2/catch_all.hpp>
#include <KernelCore/KC_Context.h>

int main (int argc, char * const argv[]) {
    KC::ScopedContext ctx;
    return Catch::Session().run( argc, argv );
}
