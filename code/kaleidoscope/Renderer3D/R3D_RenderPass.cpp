//
// > Notice: Floating Trees Inc. @ 2025
// > Create Time: 2025-10-05 21:17:40
//

#include "R3D_RenderPass.h"

namespace R3D
{
    void RenderPass::RegisterInputPin(const char* uiName, KC::String& storeInto, bool optional)
    {
        InputPin p;
        p.UI.Name = uiName;
        p.StoreInto = &storeInto;
        p.Optional = optional;

        mPins.Inputs.push_back(p);
    }

    void RenderPass::RegisterOutputPin(const char* uiName, const char* pinName)
    {
        OutputPin p;
        p.UI.Name = uiName;
        p.PinName = pinName;
        
        mPins.Outputs.push_back(p);
    }

    void RenderPass::RegisterPassThroughPin(const char* uiName, KC::String& inputRef)
    {
        OutputPin p;
        p.UI.Name = uiName;
        p.PinName = nullptr; // Will be set during validation/execution
        p.IsPassThrough = true;
        p.PassThroughInputRef = &inputRef;
        
        mPins.Outputs.push_back(p);
    }

    void RenderPass::RegisterInPlacePin(const char* inputUIName, const char* outputUIName, KC::String& storeInto, bool optional)
    {
        RegisterInputPin(inputUIName, storeInto, optional);
        RegisterPassThroughPin(outputUIName, storeInto);
    }

    bool RenderPass::ValidatePins(KC::String* err) const
    {
        for (auto& in : mPins.Inputs) {
            if (!in.Optional && (!in.StoreInto || in.StoreInto->empty())) {
                if (err) *err = "Missing input: " + in.UI.Name;
                return false;
            }
        }
        
        // Validate pass-through pins
        for (auto& out : mPins.Outputs) {
            if (out.IsPassThrough) {
                if (!out.PassThroughInputRef || out.PassThroughInputRef->empty()) {
                    if (err) *err = "Pass-through output '" + out.UI.Name + "' references empty input";
                    return false;
                }
                // Update the pass-through pin to reference the input resource
                const_cast<OutputPin&>(out).PinName = out.PassThroughInputRef->c_str();
            }
        }
        
        return true;
    }
}
