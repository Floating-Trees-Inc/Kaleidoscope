//
// > Notice: Floating Trees Inc. @ 2025
// > Create Time: 2025-10-05 21:17:40
//

#pragma once

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
    };

    struct OutputPin
    {
        PinUI UI;
        const char* PinName = nullptr;
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
        virtual ~RenderPass() = default;

        void RegisterInputPin(const char* uiName, KC::String& storeInto, bool optional = false);
        void RegisterOutputPin(const char* uiName, const char* pinName);

        bool ValidatePins(KC::String* err = nullptr) const;

        virtual void Execute(const RenderInfo& info) = 0;

        PinSet& Pins() { return mPins; }
        const KC::String& GetName() const { return mName; }
    protected:
        KC::String mName;
        PinSet mPins;
    };
}
