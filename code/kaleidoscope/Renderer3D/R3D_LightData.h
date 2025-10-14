//
// > Notice: Floating Trees Inc. @ 2025
// > Create Time: 2025-10-12 13:34:40
//

#include <World/Nodes/World_LightNode.h>

namespace R3D
{
    constexpr uint MAX_POINT_LIGHTS = 16384;
    constexpr uint MAX_SPOT_LIGHTS = 16384;

    struct PointLight
    {
        KGPU::float3 Position;
        float Radius;

        KGPU::float3 Color;
        float Intensity;
    };

    struct SpotLight
    {
        KGPU::float3 Position;
        float Size;

        KGPU::float3 Forward;
        float Angle;

        KGPU::float3 Color;
        float Intensity;
    };

    struct DirectionalLight
    {
        KGPU::float3 Direction;
        float Intensity;

        KGPU::float3 Color;
        float Pad;
    };

    class LightData
    {
    public:
        LightData();
        ~LightData();

        void BuildListsAndUpload(uint frameIndex);

        KGPU::IBuffer* GetPointLightBuffer(uint frameIndex) { return mPointLightBuffer[frameIndex]; }
        KGPU::IBufferView* GetPointLightBufferView(uint frameIndex) { return mPointLightBufferView[frameIndex]; }

        KGPU::IBuffer* GetSpotLightBuffer(uint frameIndex) { return mSpotLightBuffer[frameIndex]; }
        KGPU::IBufferView* GetSpotLightBufferView(uint frameIndex) { return mSpotLightBufferView[frameIndex]; }

        KGPU::IBuffer* GetSunBuffer(uint frameIndex) { return mSunBuffer[frameIndex]; }
        KGPU::IBufferView* GetSunBufferView(uint frameIndex) { return mSunBufferView[frameIndex]; }

        uint GetPointLightCount() { return mPointLightCount; }
        uint GetSpotLightCount() { return mSpotLightCount; }
        DirectionalLight GetSun() { return mSun; }
    private:
        KGPU::IBuffer* mPointLightBuffer[KGPU::FRAMES_IN_FLIGHT];
        KGPU::IBufferView* mPointLightBufferView[KGPU::FRAMES_IN_FLIGHT];

        KGPU::IBuffer* mSpotLightBuffer[KGPU::FRAMES_IN_FLIGHT];
        KGPU::IBufferView* mSpotLightBufferView[KGPU::FRAMES_IN_FLIGHT];

        KGPU::IBuffer* mSunBuffer[KGPU::FRAMES_IN_FLIGHT];
        KGPU::IBufferView* mSunBufferView[KGPU::FRAMES_IN_FLIGHT];

        uint mPointLightCount;
        uint mSpotLightCount;
        DirectionalLight mSun;
    };
}
