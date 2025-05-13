#pragma once

class EditorMenu;

class EditorMenu : public EditorGui
{
public:
    EditorMenu();
    virtual ~EditorMenu();

private:
    virtual void OnDrawGui() override final;

private:
    /* 재정의 가능 */
    virtual void OnTickGui() override {};
    virtual void OnStartGui() override {};
    virtual void OnEndGui() override {};
    virtual void OnMenu() {};
};