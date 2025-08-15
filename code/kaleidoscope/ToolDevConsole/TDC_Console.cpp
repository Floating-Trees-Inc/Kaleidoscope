//
// > Notice: Floating Trees Inc. @ 2025
// > Create Time: 2025-08-15 18:38:52
//

#include "TDC_Console.h"

#include <algorithm>
#include <cctype>
#include <cstring>
#include <cmath>
#include <charconv>

#include <KernelInput/KI_InputSystem.h>

#undef min
#undef max

namespace TDC
{
    static bool gRequestOpenAC = false;
    static bool gRequestCloseAC = false;

    static int stricmp_portable(const char* a, const char* b) {
    #if defined(KD_WINDOWS)
        return ::_stricmp(a, b);
    #else
        return ::strcasecmp(a, b);
    #endif
    }

    static inline float EaseInOutCubic(float t)
    {
        t = std::clamp(t, 0.0f, 1.0f);
        return (t < 0.5f) ? 4.0f * t * t * t
                          : 1.0f - std::pow(-2.0f * t + 2.0f, 3.0f) * 0.5f;
    }

    static bool ParseInt(const KC::String& s, int& out)
    {
        if (s.empty()) return false;
        const char* b = s.c_str();
        const char* e = b + s.size();
        std::from_chars_result r = std::from_chars(b, e, out);
        return r.ec == std::errc() && r.ptr == e;
    }

    static bool ParseFloat(const KC::String& s, float& out)
    {
        if (s.empty()) return false;
        const char* b = s.c_str();
        const char* e = b + s.size();
        #if defined(__cpp_lib_to_chars) && __cpp_lib_to_chars >= 201611L
        std::from_chars_result r = std::from_chars(b, e, out);
        if (r.ec == std::errc() && r.ptr == e) return true;
        #endif
        char* endp = nullptr;
        out = std::strtof(b, &endp);
        return endp == e && endp != b;
    }

    static bool ParseBool(const KC::String& s, bool& out)
    {
        if (s.empty()) return false;
        if (s.size() == 1 && (s[0] == '0' || s[0] == '1')) { out = (s[0] == '1'); return true; }
        KC::String v; v.resize(s.size());
        for (uint64 i = 0; i < s.size(); ++i) v[i] = (char)std::tolower((unsigned char)s[i]);
        if (v == "true" || v == "on" || v == "yes") { out = true; return true; }
        if (v == "false" || v == "off" || v == "no") { out = false; return true; }
        int n;
        if (ParseInt(s, n)) { out = (n != 0); return true; }
        return false;
    }

    static void ExtractCurrentToken(const char* buf, int cursorPos, int& tokBegin, KC::String& token)
    {
        int start = cursorPos - 1;
        while (start >= 0 && !isspace((unsigned char)buf[start])) --start;
        tokBegin = start + 1;
        token.assign(buf + tokBegin, cursorPos - tokBegin);
    }

    Console::Data Console::sData;

    void Console::Initialize()
    {
        KC::Context::sContext.logger->HookCallback([&](KC::LogLevel level, const KC::String& msg) {
            LogEntry entry;
            entry.Message = msg;
            switch (level) {
                case KC::LogLevel::kInfo:     entry.Color = ImColor(0xFF66FF66); break; // green
                case KC::LogLevel::kWarn:     entry.Color = ImColor(0xFFFFD966); break; // amber
                case KC::LogLevel::kError:    entry.Color = ImColor(0xFFFF6666); break; // red
                case KC::LogLevel::kWhatever: entry.Color = ImColor(0xFF66CCFF); break; // cyan
                case KC::LogLevel::kFatal:    entry.Color = ImColor(0xFFFF3366); break; // pink/red
            }
            sData.Log.push_back(entry);
        });
    }

    void Console::AddVariable(const KC::String& name, int& i)
    {
        ConsoleCommand cmd = {};
        cmd.Name = name;
        cmd.Type = ConsoleCommandType::kVariable;
        cmd.VariableType = ConsoleVariableType::kInteger;
        cmd.Integer = &i;
        sData.Registry.push_back(cmd);
    }

    void Console::AddVariable(const KC::String& name, float& f)
    {
        ConsoleCommand cmd = {};
        cmd.Name = name;
        cmd.Type = ConsoleCommandType::kVariable;
        cmd.VariableType = ConsoleVariableType::kFloat;
        cmd.Float = &f;
        sData.Registry.push_back(cmd);
    }

    void Console::AddVariable(const KC::String& name, KC::String& s)
    {
        ConsoleCommand cmd = {};
        cmd.Name = name;
        cmd.Type = ConsoleCommandType::kVariable;
        cmd.VariableType = ConsoleVariableType::kString;
        cmd.String = &s;
        sData.Registry.push_back(cmd);
    }

    void Console::AddVariable(const KC::String& name, bool& b)
    {
        ConsoleCommand cmd = {};
        cmd.Name = name;
        cmd.Type = ConsoleCommandType::kVariable;
        cmd.VariableType = ConsoleVariableType::kBoolean;
        cmd.Boolean = &b;
        sData.Registry.push_back(cmd);
    }

    void Console::Draw(float dt, int width, int height)
    {
        if (ImGui::IsKeyPressed(ImGuiKey_F11, false)) {
            sData.Opened = !sData.Opened;
            if (sData.Opened) sData.WantFocus = true;
        }

        const float kAnimDuration = 0.12f;
        float dir = sData.Opened ? 1.0f : -1.0f;
        sData.Animation += dir * (dt / kAnimDuration);
        sData.Animation = std::clamp(sData.Animation, 0.0f, 1.0f);

        float t = EaseInOutCubic(sData.Animation);
        float h = std::floor(t * (height * 0.40f));

        if (h <= 1.0f && !sData.Opened)
            return;

        ImGui::SetNextWindowPos(ImVec2(0, 0));
        ImGui::SetNextWindowSize(ImVec2((float)width, h));
        ImGui::SetNextWindowBgAlpha(0.80f * t);

        ImGuiWindowFlags flags =
            ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoResize |
            ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoSavedSettings |
            ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoScrollbar |
            ImGuiWindowFlags_NoBringToFrontOnFocus;

        if (ImGui::Begin("##DevConsole", nullptr, flags)) {
            if (t > 0.2f) {
                ImGui::GetIO().WantCaptureKeyboard = true;
                ImGui::GetIO().WantTextInput = true;
            }

            ImGui::BeginChild("##log", ImVec2(0, -ImGui::GetFrameHeightWithSpacing() * 1.5f), true);
            ImGuiListClipper clipper;
            clipper.Begin((int)sData.Log.size());
            while (clipper.Step()) {
                for (int i = clipper.DisplayStart; i < clipper.DisplayEnd; ++i)
                    ImGui::TextColored(sData.Log[i].Color, sData.Log[i].Message.c_str());
            }
            ImGui::EndChild();

            ImGuiInputTextFlags itf =
                ImGuiInputTextFlags_EnterReturnsTrue |
                ImGuiInputTextFlags_CallbackHistory |
                ImGuiInputTextFlags_CallbackCompletion |
                ImGuiInputTextFlags_CallbackEdit;

            ImGui::SetNextItemWidth(-1);
            ImGui::SetKeyboardFocusHere();
            bool activated = ImGui::InputText("##input", sData.Input, IM_ARRAYSIZE(sData.Input), itf, TextCallback, nullptr);
            if (activated) {
                KC::String line = sData.Input;
                if (!line.empty())
                    Execute(line);
                sData.Input[0] = '\0';
                sData.HistoryPos = -1;
                gRequestCloseAC = true;
            }

            if (gRequestOpenAC) {
                ImVec2 min = ImGui::GetItemRectMin();
                ImVec2 size = ImGui::GetItemRectSize();
                ImGui::SetNextWindowPos(ImVec2(min.x, min.y + size.y));
                ImGui::OpenPopup("##ac");
                gRequestOpenAC = false;
                if (sData.AcIndex < 0) sData.AcIndex = 0;
                if (sData.AcIndex >= (int)sData.Candidates.size()) sData.AcIndex = (int)sData.Candidates.size() - 1;
            } else if (ImGui::IsPopupOpen("##ac")) {
                ImVec2 min = ImGui::GetItemRectMin();
                ImVec2 size = ImGui::GetItemRectSize();
                ImGui::SetNextWindowPos(ImVec2(min.x, min.y + size.y));
            }

            if (ImGui::IsPopupOpen("##ac")) {
                if (ImGui::IsKeyPressed(ImGuiKey_DownArrow, true) && !sData.Candidates.empty()) {
                    sData.AcIndex = (sData.AcIndex + 1) % (int)sData.Candidates.size();
                }
                if (ImGui::IsKeyPressed(ImGuiKey_UpArrow, true) && !sData.Candidates.empty()) {
                    sData.AcIndex = (sData.AcIndex - 1 + (int)sData.Candidates.size()) % (int)sData.Candidates.size();
                }
            }

            if (ImGui::BeginPopup("##ac")) {
                for (int idx = 0; idx < (int)sData.Candidates.size(); ++idx) {
                    bool sel = (idx == sData.AcIndex);
                    if (ImGui::Selectable(sData.Candidates[idx].c_str(), sel, ImGuiSelectableFlags_DontClosePopups)) {
                        sData.AcIndex = idx;
                        int i = 0; while (sData.Input[i] && !isspace((unsigned char)sData.Input[i])) i++;
                        KC::String replaced = sData.Candidates[idx] + KC::String(sData.Input + i);
                        std::strncpy(sData.Input, replaced.c_str(), sizeof(sData.Input));
                        sData.Input[sizeof(sData.Input) - 1] = 0;
                        ImGui::CloseCurrentPopup();
                        gRequestCloseAC = false;
                        sData.WantFocus = true;
                    }
                }
                if (sData.Candidates.empty()) {
                    ImGui::TextDisabled("No matches");
                }
                if (gRequestCloseAC) {
                    ImGui::CloseCurrentPopup();
                    gRequestCloseAC = false;
                }
                ImGui::EndPopup();
            }
        }
        ImGui::End();
    }

    int Console::TextCallback(ImGuiInputTextCallbackData* data)
    {
        switch (data->EventFlag) {
            case ImGuiInputTextFlags_CallbackHistory: {
                if (data->EventKey == ImGuiKey_UpArrow) {
                    if (sData.HistoryPos == -1) 
                        sData.HistoryPos = (int)sData.History.size() - 1;
                    else if (sData.HistoryPos > 0) 
                        sData.HistoryPos--;
                } else if (data->EventKey == ImGuiKey_DownArrow) {
                    if (sData.HistoryPos != -1) {
                        if (++sData.HistoryPos >= (int)sData.History.size()) 
                            sData.HistoryPos = -1;
                    }
                }
                if (sData.HistoryPos >= 0) {
                    data->DeleteChars(0, data->BufTextLen);
                    data->InsertChars(0, sData.History[sData.HistoryPos].c_str());
                }
                break;
            }
            case ImGuiInputTextFlags_CallbackCompletion: {
                int tokBegin; KC::String token;
                ExtractCurrentToken(data->Buf, data->CursorPos, tokBegin, token);
                BuildCandidates(token);
                if (ImGui::IsPopupOpen("##ac") && !sData.Candidates.empty() && sData.AcIndex >= 0 && sData.AcIndex < (int)sData.Candidates.size()) {
                    KC::String c = sData.Candidates[(uint64)sData.AcIndex];
                    data->DeleteChars(tokBegin, data->CursorPos - tokBegin);
                    data->InsertChars(tokBegin, c.c_str());
                    gRequestCloseAC = true;
                    sData.WantFocus = true;
                } else {
                    sData.AcIndex = 0;
                    if (!sData.Candidates.empty()) {
                        KC::String lcp = sData.Candidates[0];
                        for (auto& s : sData.Candidates) {
                            uint64 k = 0;
                            while (k < lcp.size() && k < s.size() && tolower(lcp[k]) == tolower(s[k])) k++;
                            lcp.resize(k);
                        }
                        data->DeleteChars(tokBegin, data->CursorPos - tokBegin);
                        data->InsertChars(tokBegin, lcp.c_str());
                        gRequestOpenAC = true;
                    } else {
                        gRequestCloseAC = true;
                    }
                }
                break;
            }
            case ImGuiInputTextFlags_CallbackEdit: {
                int tokBegin; KC::String token;
                ExtractCurrentToken(data->Buf, data->CursorPos, tokBegin, token);
                bool tokenChanged = (token != sData.LastToken);
                sData.LastToken = token;
                BuildCandidates(token);
                if (!token.empty() && !sData.Candidates.empty()) {
                    if (tokenChanged) sData.AcIndex = 0;
                    if (sData.AcIndex >= (int)sData.Candidates.size()) 
                        sData.AcIndex = (int)sData.Candidates.size() - 1;
                    gRequestOpenAC = true;
                } else {
                    gRequestCloseAC = true;
                }
                break;
            }
        }
        return 0;
    }

    void Console::Execute(const KC::String& line)
    {
        sData.Log.push_back(LogEntry(KC::String("> " + line), ImColor(0xFF000000)));
        sData.History.emplace_back(line);

        uint64 sp = line.find_first_of(" \t");
        KC::String cmd = (sp == KC::String::npos) ? line : line.substr(0, sp);
        KC::String args = (sp == KC::String::npos) ? "" : line.substr(sp + 1);

        auto it = std::find_if(sData.Registry.begin(), sData.Registry.end(),
            [&](const ConsoleCommand& c){ return stricmp_portable(c.Name.c_str(), cmd.c_str()) == 0; });

        if (it == sData.Registry.end()) {
            float best = 0.f;
            int bestIdx = -1;
            for (int i = 0; i < (int)sData.Registry.size(); i++){
                float s = FuzzyScore(cmd.c_str(), sData.Registry[i].Name.c_str());
                if (s > best) { best = s; bestIdx = i; }
            }
            if (bestIdx >= 0 && best > 0.6f)
                it = sData.Registry.begin() + bestIdx;
        }

        if (it == sData.Registry.end()) {
            sData.Log.emplace_back("Unknown command.");
            return;
        }

        switch (it->Type)
        {
            case ConsoleCommandType::kFunction: {
                KD_WARN("Console commands are not supported yet!");
                break;
            }
            case ConsoleCommandType::kVariable: {
                switch (it->VariableType)
                {
                    case ConsoleVariableType::kBoolean: {
                        if (args.empty()) {
                            *it->Boolean = !(*it->Boolean);
                        } else {
                            bool v;
                            if (ParseBool(args, v)) { *it->Boolean = v; }
                            else sData.Log.emplace_back("Invalid boolean.");
                        }
                        break;
                    }
                    case ConsoleVariableType::kFloat: {
                        if (args.empty()) {
                            sData.Log.emplace_back(it->Name + KC::String(" = ") + std::to_string(*it->Float).c_str());
                        } else {
                            float v;
                            if (ParseFloat(args, v)) { *it->Float = v; }
                            else sData.Log.emplace_back("Invalid float.");
                        }
                        break;
                    }
                    case ConsoleVariableType::kInteger: {
                        if (args.empty()) {
                            sData.Log.emplace_back(it->Name + KC::String(" = ") + std::to_string(*it->Integer).c_str());
                        } else {
                            int v;
                            if (ParseInt(args, v)) { *it->Integer = v; }
                            else sData.Log.emplace_back("Invalid integer.");
                        }
                        break;
                    }
                    case ConsoleVariableType::kString: {
                        if (args.empty()) {
                            sData.Log.emplace_back(it->Name + KC::String(" = ") + *it->String);
                        } else {
                            *it->String = args;
                        }
                        break;
                    }
                }
                break;
            }
        }
    }

    void Console::BuildCandidates(const KC::String& token)
    {
        sData.Candidates.clear();
        if (token.empty()) {
            for (auto& c : sData.Registry)
                sData.Candidates.push_back(c.Name);
        } else {
            struct Item { KC::String s; float score; };
            KC::Array<Item> tmp;
            for (auto& c : sData.Registry) {
                float sc = FuzzyScore(token.c_str(), c.Name.c_str());
                if (sc > 0.15f)
                    tmp.push_back({c.Name, sc});
            }
            std::sort(tmp.begin(), tmp.end(), [](auto& a, auto& b){ return a.score > b.score; });
            for (auto& t : tmp)
                sData.Candidates.push_back(t.s);
            if (sData.Candidates.size() > 16)
                sData.Candidates.resize(16);
        }
    }

    float Console::FuzzyScore(const char* pat, const char* str)
    {
        int pi = 0;
        float score = 0.f;
        int streak = 0;
        for (int si = 0; str[si]; si++) {
            if (!pat[pi]) break;
            if (tolower(str[si]) == tolower(pat[pi])) {
                score += 1.f + 0.1f * streak;
                streak++; pi++;
            } else {
                score -= 0.05f; streak = 0;
            }
        }
        if (pat[pi]) score -= 0.5f;
        float len = (float)std::max((int)strlen(str), (int)strlen(pat));
        return std::clamp(score / (len * 1.2f), 0.f, 1.f);
    }
}
