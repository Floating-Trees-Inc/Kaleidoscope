//
// > Notice: Floating Trees Inc. @ 2025
// > Create Time: 2025-10-05 21:17:40
//

#pragma once

#include <KernelCore/KC_UUID.h>

#include "R3D_RenderInfo.h"

namespace R3D
{
    enum class PinKind : uint8
    {
        kInput,
        kOutput
    };

    struct PinUI { KC::String Name; };

    struct InputPin
    {
        PinUI UI;
        KC::String* StoreInto = nullptr;
        bool Optional = false;
        const char* PinName = nullptr;

        KC::UUID UUID = KC::NewUUID();
    };

    struct OutputPin
    {
        PinUI UI;
        const char* PinName = nullptr;
        
        // For pass-through pins: reference an input pin's resource instead of creating new one
        bool IsPassThrough = false;
        KC::String* PassThroughInputRef = nullptr;

        KC::UUID UUID = KC::NewUUID();
    };

    struct PinSet
    {
        KC::Array<InputPin> Inputs;
        KC::Array<OutputPin> Outputs;

        OutputPin& FindOutput(const char* name)
        {
            for (auto& pin : Outputs)
            {
                if (pin.PinName == name)
                    return pin;
            }
            KD_WARN("Output pin not found");
            return Outputs[0];
        }

        OutputPin& FindOutputByUIName(const char* name)
        {
            for (auto& pin : Outputs)
            {
                if (pin.UI.Name == name)
                    return pin;
            }
            KD_WARN("Output pin not found");
            return Outputs[0];
        }
    };

    class RenderPass : public KC::RefCounted
    {
    public:
        RenderPass(const char* name, KC::UUID uuid = KC::NewUUID())
            : mName(name), mUUID(uuid)
        {
        }

        virtual ~RenderPass() = default;

        void RegisterInputPin(const char* uiName, KC::String& storeInto, bool optional = false);
        void RegisterOutputPin(const char* uiName, const char* pinName);
        void RegisterPassThroughPin(const char* uiName, KC::String& inputRef);
        void RegisterInPlacePin(const char* inputUIName, const char* outputUIName, KC::String& storeInto, bool optional = false);

        bool ValidatePins(KC::String* err = nullptr) const;

        virtual void Execute(const RenderInfo& info) = 0;

        PinSet& Pins() { return mPins; }
        const KC::String& GetName() const { return mName; }
    protected:
        KC::UUID mUUID;
        KC::String mName;
        PinSet mPins;
    };
}
