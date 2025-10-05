//
// > Notice: Floating Trees Inc. @ 2025
// > Create Time: 2025-10-05 16:24:00
//

#pragma once

#include "Editor_Panel.h"

namespace Editor
{
    class PanelManager
    {
    public:
        PanelManager();
        ~PanelManager();

        template<typename T>
        T* RegisterPanel()
        {
            static_assert(std::is_base_of_v<Panel, T>, "T must derive from Editor::Panel");
            const uint64 key = TypeKey<T>();
            if (mPanels.find(key) != mPanels.end())
                return (T*)mPanels[key]; // already registered; no-op (could KC_WARN here)

            Panel* panel = KC_NEW(T);
            mPanels[key] = panel;
            return (T*)mPanels[key];
        }

        template<typename T>
        T* GetPanel()
        {
            static_assert(std::is_base_of_v<Panel, T>, "T must derive from Editor::Panel");
            const uint64 key = TypeKey<T>();
            auto it = mPanels.find(key);
            if (it == mPanels.end())
                return nullptr;
            return static_cast<T*>(it->second);
        }

        template<typename T>
        void RemovePanel()
        {
            static_assert(std::is_base_of_v<Panel, T>, "T must derive from Editor::Panel");
            const uint64 key = TypeKey<T>();
            auto it = mPanels.find(key);
            if (it == mPanels.end())
                return; // not found; no-op

            KC_DELETE(it->second);
            mPanels.erase(it);
        }

        template<typename T>
        void OpenPanel()
        {
            static_assert(std::is_base_of_v<Panel, T>, "T must derive from Editor::Panel");
            T* panel = GetPanel<T>();
            if (!panel)
            {
                RegisterPanel<T>();
                panel = GetPanel<T>();
                if (!panel) return; // allocation failed; bail
            }
            panel->Open();
        }

        KC::Array<Panel*> GetPanels()
        {
            KC::Array<Panel*> panels;
            for (auto& kv : mPanels) panels.push_back(kv.second);
            return panels;
        }
    private:
        template<typename T>
        static uint64 TypeKey()
        {
            return static_cast<uint64>(typeid(T).hash_code());
        }
    
        KC::HashMap<uint64, Panel*> mPanels;
    };
}
