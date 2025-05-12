#pragma once

class EditorPlayMenu : public EditorMenu
{
public:
    EditorPlayMenu()
    {
        SetCallOrder(0);
        SetPath("Play");
    }
    virtual ~EditorPlayMenu() = default;

public:
    virtual void OnMenu() override;
};