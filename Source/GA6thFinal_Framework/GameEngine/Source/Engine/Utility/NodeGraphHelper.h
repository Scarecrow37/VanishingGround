#pragma once

namespace ed = ax::NodeEditor;

namespace NodeGraph
{
    class Pin;
    class Node;
    class Link;
    ImU32 GetColor4ToImU32(const ImVec4& color);
    ImU32 GetColor4ToImU32(int* color);
    ImU32 GetColor4ToImU32(float r, float g, float b, float a = 1.0f);

    class NodeEditorStyleColorBuilder
    {
    public:
        NodeEditorStyleColorBuilder() = default;
        ~NodeEditorStyleColorBuilder() { PopStyleColor(); }

    public:
        inline void PushStyleColor(ed::StyleColor style, const ImVec4& color)
        {
            ed::PushStyleColor(style, color);
            ++_pushCount;
        }
        inline void PopStyleColor() { ed::PopStyleColor(_pushCount); }

    private:
        int _pushCount = 0;

    };

    class NodeEditorStyleVarBuilder
    {
    public:
        NodeEditorStyleVarBuilder() = default;
        ~NodeEditorStyleVarBuilder() { PopStyleVar(); }

    public:
        template <typename T>
        inline void PushStyleVar(ed::StyleVar style, const T& value)
        {
            ed::PushStyleVar(style, value);
            ++_pushCount;
        }
        inline void PopStyleVar() { ed::PopStyleVar(_pushCount); _pushCount = 0; }

    private:
        int _pushCount = 0;

    };
} // namespace NodeGraph
