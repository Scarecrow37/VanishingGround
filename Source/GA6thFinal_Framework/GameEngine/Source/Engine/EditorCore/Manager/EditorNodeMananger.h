#pragma once

class EditorNodeMananger
{
public:
    EditorNodeMananger()  = default;
    ~EditorNodeMananger() = default;

public:
    void Initialize();
    void DrawGui();
    void UnInitialize();

private:
    ed::EditorContext* _editor = nullptr;

};