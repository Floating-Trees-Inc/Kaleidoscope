//
// > Notice: Floating Trees Inc. @ 2025
// > Create Time: 2025-10-05 12:08:25
//

#include "Gfx_Model.h"
#include "Gfx_CacheManager.h"

namespace Gfx
{
    Model::Model(const KC::String& path)
    {
        auto mesh = KDA::MeshLoader::LoadMeshFromFile(path);
        for (auto& node : mesh.Nodes) {
            for (auto& prim : node.Primitives) {
                Submesh submesh;
                submesh.Primitive = KC_NEW(MeshPrimitive, prim);
                submesh.Material = KC_NEW(Material);
                submesh.Material->FromMaterial(mesh.Materials[prim.MaterialIndex]);
            }
        }
    }

    Model::~Model()
    {
        for (auto& mesh : Submeshes) {
            KC_DELETE(mesh.Primitive);
            KC_DELETE(mesh.Material);
        }
    }
}
