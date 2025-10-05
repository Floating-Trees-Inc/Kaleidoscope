//
// > Notice: Floating Trees Inc. @ 2025
// > Create Time: 2025-10-05 16:24:00
//

#include "Editor_PanelManager.h"

namespace Editor
{
    PanelManager::PanelManager()
    {
        // Nothing here yet
    }

    PanelManager::~PanelManager()
    {
        for (auto& [_, panel] : mPanels) {
            KC_DELETE(panel);
        }
        mPanels.clear();
    }
}
