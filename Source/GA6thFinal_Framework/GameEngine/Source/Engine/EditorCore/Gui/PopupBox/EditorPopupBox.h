#pragma once

class EditorPopupBox
{
    using PopupBoxFunc = std::function<void()>;
public:
    EditorPopupBox(std::string_view name, ImGuiWindowFlags flags, std::function<void()> content);
    EditorPopupBox(std::string_view name, std::function<void()> content);
    ~EditorPopupBox() = default;

    inline void                 SetFlags(ImGuiWindowFlags flags) { _flags = flags; }
    inline void                 SetSize(ImVec2 size) { _initSize = size; }
    inline void                 SetContent(PopupBoxFunc content) { _content = content; }

    inline bool                 IsNull() const { return _content == nullptr; }
    inline bool                 IsOpen() const { return _isOpen; }
    inline const std::string&   GetName() const { return _name; }
    inline ImGuiWindowFlags     GetFlags() const { return _flags; }
    inline const ImVec2&        GetSize() const { return _initSize; }
    inline const PopupBoxFunc&  GetContent() const { return _content; }

private:
    std::string         _name;
    ImGuiWindowFlags    _flags;
    ImVec2              _initSize;
    PopupBoxFunc        _content;
    bool                _isOpen;
    bool                _isStart;
};