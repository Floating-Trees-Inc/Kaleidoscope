//
// > Notice: Floating Trees Inc. @ 2025
// > Create Time: 2025-10-12 13:34:40
//

#include "R3D_LightData.h"
#include "World/Nodes/World_LightNode.h"
#include "World/World_Manager.h"

#include <Graphics/Gfx_Manager.h>

namespace R3D
{
    LightData::LightData()
    {
        KGPU::IDevice* device = Gfx::Manager::GetDevice();

        for (int i = 0; i < KGPU::FRAMES_IN_FLIGHT; i++) {
           mPointLightBuffer[i] = device->CreateBuffer(KGPU::BufferDesc(sizeof(PointLight) * MAX_POINT_LIGHTS, sizeof(PointLight), KGPU::BufferUsage::kStaging | KGPU::BufferUsage::kShaderRead));
           mPointLightBuffer[i]->SetName("Point Light Buffer");
           mPointLightBufferView[i] = device->CreateBufferView(KGPU::BufferViewDesc(mPointLightBuffer[i], KGPU::BufferViewType::kStructured));

           mSpotLightBuffer[i] = device->CreateBuffer(KGPU::BufferDesc(sizeof(PointLight) * MAX_POINT_LIGHTS, sizeof(PointLight), KGPU::BufferUsage::kStaging | KGPU::BufferUsage::kShaderRead));
           mSpotLightBuffer[i]->SetName("Spot Light Buffer");
           mSpotLightBufferView[i] = device->CreateBufferView(KGPU::BufferViewDesc(mSpotLightBuffer[i], KGPU::BufferViewType::kStructured));

           mSunBuffer[i] = device->CreateBuffer(KGPU::BufferDesc(sizeof(DirectionalLight), sizeof(DirectionalLight), KGPU::BufferUsage::kStaging | KGPU::BufferUsage::kShaderRead));
           mSunBuffer[i]->SetName("Sun Buffer");
           mSunBufferView[i] = device->CreateBufferView(KGPU::BufferViewDesc(mSunBuffer[i], KGPU::BufferViewType::kStructured));
        }
    }

    LightData::~LightData()
    {
        for (int i = 0; i < KGPU::FRAMES_IN_FLIGHT; i++) {
            KC_DELETE(mPointLightBuffer[i]);
            KC_DELETE(mPointLightBufferView[i]);
            KC_DELETE(mSpotLightBuffer[i]);
            KC_DELETE(mSpotLightBufferView[i]);
            KC_DELETE(mSunBuffer[i]);
            KC_DELETE(mSunBufferView[i]);
        }
    }

    void LightData::BuildListsAndUpload(uint frameIndex)
    {
        KC::Array<PointLight> pointLightList;
        KC::Array<SpotLight> spotLightList;
        DirectionalLight sunLight;

        pointLightList.reserve(MAX_POINT_LIGHTS);
        spotLightList.reserve(MAX_SPOT_LIGHTS);

        auto group = World::Manager::GetGroups().GetGroup(World::NodeGroupType::kLights);
        for (auto& node : group) {
            World::LightNode* lightNode = reinterpret_cast<World::LightNode*>(node);
            switch (lightNode->GetType()) {
                case World::LightType::kPoint:
                    PointLight pointLight;
                    pointLight.Color = lightNode->GetColor();
                    pointLight.Intensity = lightNode->GetIntensity();
                    pointLight.Radius = lightNode->GetRadius();
                    pointLight.Position = lightNode->GetPosition();

                    pointLightList.push_back(pointLight);
                    break;
                case World::LightType::kSpot:
                    SpotLight spotLight;
                    spotLight.Color = lightNode->GetColor();
                    spotLight.Intensity = lightNode->GetIntensity();
                    spotLight.Position = lightNode->GetPosition();
                    spotLight.Forward = lightNode->GetForward();

                    spotLightList.push_back(spotLight);
                    break;
                case World::LightType::kDirectional:
                    DirectionalLight dirLight;
                    dirLight.Color = lightNode->GetColor();
                    dirLight.Intensity = lightNode->GetIntensity();
                    dirLight.Direction = lightNode->GetForward();

                    sunLight = dirLight;
                    break;
            }
        }

        void* mPtr = mPointLightBuffer[frameIndex]->Map();
        memcpy(mPtr, pointLightList.data(), sizeof(PointLight) * pointLightList.size());
        mPointLightBuffer[frameIndex]->Unmap();

        mPtr = mSpotLightBuffer[frameIndex]->Map();
        memcpy(mPtr, spotLightList.data(), sizeof(SpotLight) * spotLightList.size());
        mSpotLightBuffer[frameIndex]->Unmap();

        mPtr = mSunBuffer[frameIndex]->Map();
        memcpy(mPtr, &sunLight, sizeof(DirectionalLight));
        mSunBuffer[frameIndex]->Unmap();
    }
}
