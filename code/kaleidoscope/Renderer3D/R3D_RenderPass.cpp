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

    bool RenderPass::ValidatePins(KC::String* err) const
    {
        for (auto& in : mPins.Inputs) {
            if (!in.Optional && (!in.StoreInto || in.StoreInto->empty())) {
                if (err) *err = "Missing input: " + in.UI.Name;
                return false;
            }
        }
        return true;
    }
}
