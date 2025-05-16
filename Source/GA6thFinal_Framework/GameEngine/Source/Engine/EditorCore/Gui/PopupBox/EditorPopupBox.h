#pragma once

class EditorPopupBox
{
public:
    EditorPopupBox(std::string_view name, std::function<void()> content) : name(name), content(content) {}
    ~EditorPopupBox() = default;

    inline bool        IsNull() const { return content == nullptr; }
    inline bool        IsOpen() const { return isOpen; }
    inline const auto& GetName() const { return name; }
    inline const auto& GetContent() const { return content; }

private:
    std::string           name    = "";
    std::function<void()> content = nullptr;
    bool                  isOpen  = true;
    bool                  isStart = false;
};