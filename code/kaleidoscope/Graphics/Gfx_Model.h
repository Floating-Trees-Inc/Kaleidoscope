//
// > Notice: Floating Trees Inc. @ 2025
// > Create Time: 2025-10-05 12:08:25
//

#pragma once

#include "Gfx_Material.h"
#include "Gfx_MeshPrimitive.h"

#include <KernelCore/KC_RefCounted.h>

namespace Gfx
{
    struct Submesh
    {
        Material* Material;
        MeshPrimitive* Primitive;  
    };

    struct Model : KC::RefCounted
    {
        Model(const KC::String& path);
        ~Model();

        KC::String Path;
        KC::Array<Submesh> Submeshes;

        bool operator==(const Model& other) {
            return other.Path == Path;
        }
    };
}
