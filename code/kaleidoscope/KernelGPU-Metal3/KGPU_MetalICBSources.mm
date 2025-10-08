
//
// > Notice: Floating Trees Inc. @ 2025
// > Create Time: 2025-07-05 19:04:09
//

static const char* sICBConversionDrawShaderSrc = R"MSL(
#include <metal_stdlib>
using namespace metal;

struct KD_DrawCmd
{
    uint drawID;
    uint vertexCount;
    uint instanceCount;
    uint vertexStart;
    uint instanceStart;
};

struct TopLevelAB_Layout
{
    uint4  push40[10];  // 10 * 16 = 160 bytes (example packing for 160B)
    uint   drawID;      // 4 bytes
};

struct arguments {
    command_buffer cmd_buffer;
};

kernel void encode_draws
    (device const KD_DrawCmd* cmdIn [[buffer(0)]],
     device const uint* drawCount [[buffer(1)]],
     device const uint* primitiveType [[buffer(2)]],
     device arguments &args,
     device TopLevelAB_Layout* argBuffer [[buffer(4)]],
     uint gid [[thread_position_in_grid]])
{
    if (gid >= *drawCount) return;

    // Perform arg buffer copy
    const KD_DrawCmd d = cmdIn[gid];
    const uint stride = 164; // 160 + 4 bytes

    device TopLevelAB_Layout* src = argBuffer;
    device TopLevelAB_Layout* dst = reinterpret_cast<device TopLevelAB_Layout*>(argBuffer + gid * stride);
    dst->drawID = d.drawID;
    if (gid > 0)
    {
        device uint8_t* dPtr = reinterpret_cast<device uint8_t*>(dst);
        device uint8_t* sPtr = reinterpret_cast<device uint8_t*>(src);
        for (uint i = 0; i < 160; ++i) dPtr[i] = sPtr[i];
    }

    uint primType = primitiveType[0];
    primitive_type pt;
    switch (primType) {
        case 0: pt = primitive_type::point; break;
        case 1: pt = primitive_type::line; break;
        case 2: pt = primitive_type::triangle; break;
    }

    // Binding 0 has push constants, binding 1 has draw ID. The push constant is 160 bytes wide so offset the buffer

    render_command cmd(args.cmd_buffer, gid);
    cmd.reset();
    cmd.set_vertex_buffer(dst, 0, 3);
    cmd.draw_primitives(pt, d.vertexStart, d.vertexCount, d.instanceCount, d.instanceStart);
}
)MSL";

static const char* sICBConversionDrawIndexedShaderSrc = R"MSL(
#include <metal_stdlib>
using namespace metal;

struct KD_DrawIndexedCmd
{
    uint drawID;
    uint indexCount;
    uint instanceCount;
    uint firstIndex;
    uint vertexOffset;
    uint firstInstance;
};

struct arguments {
    command_buffer cmd_buffer;
};

struct TopLevelAB_Layout
{
    uint4  push40[10];  // 10 * 16 = 160 bytes (example packing for 160B)
    uint   drawID;      // 4 bytes
    // implicit padding may exist; we only copy the bytes we need
};

kernel void encode_draws
    (device const KD_DrawIndexedCmd* cmdIn [[buffer(0)]],
     device const uint* drawCount [[buffer(1)]],
     device const uint* indexBuffer [[buffer(2)]], // Bro XD
     device const uint* primitiveType [[buffer(3)]],
     device arguments &args,
     device TopLevelAB_Layout* argBuffer [[buffer(5)]],
     uint gid [[thread_position_in_grid]])
{
    if (gid >= *drawCount) return;

    // Perform arg buffer copy
    const KD_DrawIndexedCmd d = cmdIn[gid];
    const uint stride = 164; // 160 + 4 bytes

    device TopLevelAB_Layout* src = argBuffer;
    device TopLevelAB_Layout* dst = reinterpret_cast<device TopLevelAB_Layout*>(argBuffer + gid * stride);
    dst->drawID = d.drawID;
    if (gid > 0)
    {
        device uint8_t* dPtr = reinterpret_cast<device uint8_t*>(dst);
        device uint8_t* sPtr = reinterpret_cast<device uint8_t*>(src);
        for (uint i = 0; i < 160; ++i) dPtr[i] = sPtr[i];
    }

    uint primType = primitiveType[0];
    primitive_type pt;
    switch (primType) {
        case 0: pt = primitive_type::point; break;
        case 1: pt = primitive_type::line; break;
        case 2: pt = primitive_type::triangle; break;
    }

    render_command cmd(args.cmd_buffer, gid);
    cmd.reset();
    cmd.set_vertex_buffer(dst, 0, 3);
    cmd.draw_indexed_primitives(pt, d.indexCount, indexBuffer, d.instanceCount, d.firstIndex, d.firstInstance);
}
)MSL";

static const char* sICBConversionDispatchShaderSrc = R"MSL(
#include <metal_stdlib>
using namespace metal;

struct KD_DispatchCmd
{
    uint x;
    uint y;
    uint z;
};

struct arguments {
    command_buffer cmd_buffer;
};

kernel void encode_dispatches
    (device const KD_DispatchCmd* cmdIn [[buffer(0)]],
     device const uint4* threadsPerGroup [[buffer(1)]],
     device arguments &args,
     uint gid [[thread_position_in_grid]])
{
    const KD_DispatchCmd d = cmdIn[0];

    compute_command cmd(args.cmd_buffer, gid);
    cmd.reset();
    cmd.concurrent_dispatch_threadgroups(uint3(d.x, d.y, d.z), uint3(threadsPerGroup[0]));
}
)MSL";
