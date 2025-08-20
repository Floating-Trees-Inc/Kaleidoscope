//
// > Notice: Floating Trees Inc. @ 2025
// > Create Time: 2025-07-23 21:12:54
//

#include "Phys_Manager.h"

#include <KernelCore/KC_Context.h>

namespace Phys
{
    namespace Layers
    {
        static constexpr uint8 NON_MOVING = 0;
        static constexpr uint8 MOVING = 1;
        static constexpr uint8 CHARACTER = 2;
        static constexpr uint8 CHARACTER_GHOST = 3;
        static constexpr uint8 TRIGGER = 4;
        static constexpr uint8 NUM_LAYERS = 5;
    };

    namespace BroadPhaseLayers
    {
        static constexpr JPH::BroadPhaseLayer NON_MOVING(0);
        static constexpr JPH::BroadPhaseLayer MOVING(1);
        static constexpr uint32 NUM_LAYERS(2);
    };

    class BPLayerInterfaceImpl final : public JPH::BroadPhaseLayerInterface
    {
    public:
        BPLayerInterfaceImpl()
        {
            // Create a mapping table from object to broad phase layer
            mObjectToBroadPhase[Layers::NON_MOVING] = BroadPhaseLayers::NON_MOVING;
            mObjectToBroadPhase[Layers::MOVING] = BroadPhaseLayers::MOVING;
            mObjectToBroadPhase[Layers::CHARACTER] = BroadPhaseLayers::MOVING;
            mObjectToBroadPhase[Layers::CHARACTER_GHOST] = BroadPhaseLayers::MOVING;
            mObjectToBroadPhase[Layers::TRIGGER] = BroadPhaseLayers::MOVING;
        }

        virtual JPH::uint GetNumBroadPhaseLayers() const override
        {
            return BroadPhaseLayers::NUM_LAYERS;
        }

        virtual JPH::BroadPhaseLayer GetBroadPhaseLayer(JPH::ObjectLayer inLayer) const override
        {
            using namespace JPH;
            JPH_ASSERT(inLayer < Layers::NUM_LAYERS);
            return mObjectToBroadPhase[inLayer];
        }
    private:
        JPH::BroadPhaseLayer					mObjectToBroadPhase[Layers::NUM_LAYERS];
    };

    class ObjectVsBroadPhaseLayerFilterImpl : public JPH::ObjectVsBroadPhaseLayerFilter
    {
    public:
        virtual bool ShouldCollide(JPH::ObjectLayer inLayer1, JPH::BroadPhaseLayer inLayer2) const override
        {
            switch (inLayer1)
            {
            case Layers::NON_MOVING:
                return inLayer2 == BroadPhaseLayers::MOVING;
            case Layers::MOVING:
                return true;
            case Layers::TRIGGER:
                return inLayer2 == BroadPhaseLayers::MOVING;
            default:
                return false;
            }
        }
    };

    class ObjectLayerPairFilterImpl : public JPH::ObjectLayerPairFilter
    {
    public:
        virtual bool ShouldCollide(JPH::ObjectLayer inObject1, JPH::ObjectLayer inObject2) const override
        {
            switch (inObject1)
            {
            case Layers::TRIGGER:
                return inObject2 == Layers::CHARACTER_GHOST || inObject2 == Layers::CHARACTER;
            case Layers::NON_MOVING:
                return inObject2 == Layers::MOVING || inObject2 == Layers::CHARACTER_GHOST || inObject2 == Layers::CHARACTER; // Non moving only collides with moving
            case Layers::MOVING:
                return true;
            case Layers::CHARACTER_GHOST:
                return inObject2 != Layers::CHARACTER;
            case Layers::CHARACTER:
                return inObject2 != Layers::CHARACTER_GHOST;
            default:
                return false;
            }
        }
    };

    BPLayerInterfaceImpl JoltBroadphaseLayerInterface = BPLayerInterfaceImpl();
    ObjectVsBroadPhaseLayerFilterImpl JoltObjectVSBroadphaseLayerFilter = ObjectVsBroadPhaseLayerFilterImpl();
    ObjectLayerPairFilterImpl JoltObjectVSObjectLayerFilter;

    void Manager::Initialize()
    {
        JPH::RegisterDefaultAllocator();
        JPH::Factory::sInstance = KC_NEW(JPH::Factory);

        JPH::RegisterTypes();

        const uint maxBodies = 4096;
        const uint numBodyMutexes = 0;
        const uint maxBodyPairs = 2048;
        const uint maxContactContraints = 2048;
        const uint availableThreads = std::thread::hardware_concurrency() - 1;

        sData.System = KC_NEW(JPH::PhysicsSystem);
        sData.System->Init(maxBodies, numBodyMutexes, maxBodyPairs, maxContactContraints, JoltBroadphaseLayerInterface, JoltObjectVSBroadphaseLayerFilter, JoltObjectVSObjectLayerFilter);

        sData.System->SetGravity(JPH::Vec3(0.0f, -3.0f, 0.0f));
        sData.BodyInterface = &sData.System->GetBodyInterface();
        sData.JobSystem = KC_NEW(JPH::JobSystemThreadPool, JPH::cMaxPhysicsJobs, JPH::cMaxPhysicsBarriers, availableThreads);

        KD_INFO("Initialized physics!");
    }

    void Manager::Shutdown()
    {
        KC_DELETE(sData.JobSystem);
        KC_DELETE(sData.System);

        JPH::UnregisterTypes();

        KC_DELETE(JPH::Factory::sInstance);
        JPH::Factory::sInstance = nullptr;
    }

    void Manager::Update()
    {
        int collisionSteps = 1;
        constexpr float minStepDuration = 1.0f / 90.0f;

        if (sData.PhysicsTimer.ToSeconds() > minStepDuration) {
            try {
                auto allocator = KC_NEW(JPH::TempAllocatorMalloc);
                CODE_BLOCK("Update") {
                    auto error = sData.System->Update(minStepDuration, collisionSteps, allocator, sData.JobSystem);
                    if (error != JPH::EPhysicsUpdateError::None) {
                        const char* errorMsg = "";
                        switch (error) {
                            case JPH::EPhysicsUpdateError::ManifoldCacheFull:
                                errorMsg = "Manifold cache full";
                                break;
                            case JPH::EPhysicsUpdateError::BodyPairCacheFull:
                                errorMsg = "Body pair cache full";
                                break;
                            case JPH::EPhysicsUpdateError::ContactConstraintsFull:
                                errorMsg = "contact constraints full";
                                break;
                        }
                        KD_ERROR("Physics error: %s", errorMsg);
                    }
                }
                KC_DELETE(allocator);
            } catch (...) {
                KD_FATAL("!! PHYSICS FATAL ERROR !!");
            }
            sData.PhysicsTimer.Restart();
        }
    }
}
