#pragma once
class OverlayPanel;
class ImageElement;

struct CharacterHUDGroup
{
    OverlayPanel* GroupPanel        = nullptr;

    OverlayPanel* PlayerHUDPanel    = nullptr;
    Vector3       PlayerPosition    = Vector3::Zero;

    OverlayPanel* EnemyHUDPanel[3]  = {nullptr, nullptr, nullptr}; // Left, Middle, Right
    Vector3       EnemyPosition[3]  = {Vector3::Zero, Vector3::Zero, Vector3::Zero};

    bool FindUI();
    bool IsValid();
    void ActiveUI(bool active);
    void RefreshUIPosition();
    bool RefreshEnemiesPosition();
};