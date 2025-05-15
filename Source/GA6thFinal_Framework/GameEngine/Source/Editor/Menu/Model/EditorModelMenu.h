#pragma once

class EditorModelMenu : public EditorMenu
{
public:
    EditorModelMenu();
    virtual ~EditorModelMenu() = default;

public:
    virtual void OnStartGui() override;
    virtual void OnMenu() override;

private:
    EditorModelDetails* _editorModelDetails;
    std::string         _save;
    std::string         _saveAs;
    std::string         _imoprt;
    std::string         _open;
};