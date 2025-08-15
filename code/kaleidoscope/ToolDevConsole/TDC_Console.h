//
// > Notice: Floating Trees Inc. @ 2025
// > Create Time: 2025-08-15 15:47:09
//

#pragma once

#include <KernelCore/KC_Context.h>

#include <functional>
#include <imgui.h>

namespace TDC
{
    enum class ConsoleCommandType
    {
        kFunction,
        kVariable
    };

    enum class ConsoleVariableType
    {
        kInteger,
        kFloat,
        kString,
        kBoolean
    };

    struct ConsoleCommand
    {
        KC::String Name;
        ConsoleCommandType Type;
        ConsoleVariableType VariableType;

        // Values
        int* Integer;
        float* Float;
        KC::String* String;
        bool* Boolean;

        // Functions
        std::function<void(const KC::String& args)> Function;
    };

    struct LogEntry
    {
        ImColor Color;
        KC::String Message;

        LogEntry() = default;
        LogEntry(KC::String message) { Message = message; Color = ImColor(0xFF000000); }
        LogEntry(KC::String message, ImColor color) { Message = message; Color = color; }
        operator KC::String() const { return Message; }
    };

    class Console
    {
    public:
        static void Initialize();

        static void AddFunction(const KC::String& name, const std::function<void(const KC::String& args)>& fn);
        static void AddVariable(const KC::String& name, int& i);
        static void AddVariable(const KC::String& name, float& f);
        static void AddVariable(const KC::String& name, KC::String& s);
        static void AddVariable(const KC::String& name, bool& b);

        static void Draw(float dt, int width, int height);
    private:
        static int TextCallback(ImGuiInputTextCallbackData* data);
        static void Execute(const KC::String& line);
        static void BuildCandidates(const KC::String& token);
        static float FuzzyScore(const char* pat, const char* str);

        static struct Data {
            bool Opened = false;
            float Animation = 0.0f;
            char Input[512] = {};
            int HistoryPos = -1;
            bool WantFocus = false;
            bool RequestOpenAC = false;
            bool RequestCloseAC = false;
            int AcIndex = 0;
            KC::String LastToken;
            bool Activated = false;

            KC::Array<KC::String> History;
            KC::Array<LogEntry> Log;
            KC::Array<KC::String> Candidates;
            KC::Array<ConsoleCommand> Registry;
        } sData;
    };
}
