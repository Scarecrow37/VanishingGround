#pragma once
class OverlayPanel;
class ImageElement;

struct CharacterHUDGroup
{
    OverlayPanel* GroupPanel       = nullptr;
    OverlayPanel* PlayerHUDPanel   = nullptr;
    OverlayPanel* EnemyHUDPanel[3] = {nullptr, nullptr, nullptr}; // Left, Middle, Right

    bool FindUI();
    bool IsValid();
};