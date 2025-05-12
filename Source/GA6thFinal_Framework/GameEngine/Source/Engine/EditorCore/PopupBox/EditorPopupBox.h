#pragma once

class EditorPopupBox
{
public:
    EditorPopupBox(std::string_view name, std::function<void()> content) : name(name), content(content) {}
    ~EditorPopupBox() = default;

    inline bool        IsNull() { return content == nullptr; }
    inline bool        IsOpen() { return isOpen; }
    inline const auto& GetName() { return name; }
    inline const auto& GetContent() { return content; }

private:
    std::string           name    = "";
    std::function<void()> content = nullptr;
    bool                  isOpen  = true;
    bool                  isStart = false;
};