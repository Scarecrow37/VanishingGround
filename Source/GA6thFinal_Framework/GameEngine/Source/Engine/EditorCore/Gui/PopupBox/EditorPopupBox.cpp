#include "pch.h"
#include "EditorPopupBox.h"

EditorPopupBox::EditorPopupBox(std::string_view name, ImGuiWindowFlags flags, std::function<void()> content) 
    : _name(name)
    , _flags(flags)
    , _initSize(ImVec2(-FLT_MAX, FLT_MAX))
    , _content(content)
    , _isOpen(true)
    , _isStart(false)
{
}
EditorPopupBox::EditorPopupBox(std::string_view name, std::function<void()> content) 
    : _name(name)
    , _flags(ImGuiWindowFlags_AlwaysAutoResize)
    , _initSize(ImVec2(-FLT_MAX, FLT_MAX))
    , _content(content)
    , _isOpen(true)
    , _isStart(false)
{
}