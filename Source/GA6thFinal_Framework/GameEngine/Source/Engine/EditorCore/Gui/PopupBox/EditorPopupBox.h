#pragma once

class EditorPopupBox
{
    using PopupBoxFunc = std::function<void()>;
public:
    EditorPopupBox(std::string_view name, std::function<void()> content) 
        : _name(name), _content(content) {}
    ~EditorPopupBox() = default;

    inline bool                 IsNull() const { return _content == nullptr; }
    inline bool                 IsOpen() const { return _isOpen; }
    inline const std::string&   GetName() const { return _name; }
    inline const PopupBoxFunc&  GetContent() const { return _content; }

private:
    std::string                 _name    = "";
    PopupBoxFunc                _content = nullptr;
    bool                        _isOpen  = true;
    bool                        _isStart = false;
};