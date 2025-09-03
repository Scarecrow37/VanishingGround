#pragma once

class EditorGameView;

class EditorGameMenu : public EditorMenu
{
public:
    EditorGameMenu();
    virtual ~EditorGameMenu() = default;

public:
    virtual void OnStartGui() override;
    virtual void OnMenu() override;

private:
    EditorGameView* _gameView;
};
