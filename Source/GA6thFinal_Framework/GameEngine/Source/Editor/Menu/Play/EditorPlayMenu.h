#pragma once

class EditorPlayMenu : public EditorMenu
{
public:
    EditorPlayMenu() = default;
    virtual ~EditorPlayMenu() = default;

public:
    virtual void OnMenu() override;
};