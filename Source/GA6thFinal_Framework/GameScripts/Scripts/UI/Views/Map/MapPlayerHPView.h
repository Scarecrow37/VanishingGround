#pragma once
#include "ViewModels/Hp/CharacterHPViewModel.h"

class TextElement;
class MapPlayerHPView : public Component
{
    USING_PROPERTY(MapPlayerHPView)

public:
    MapPlayerHPView();
    ~MapPlayerHPView() override;

public:
    void Start() override;

public:
    REFLECT_PROPERTY()

protected:
    REFLECT_FIELDS_BEGIN(Component)
    REFLECT_FIELDS_END(MapPlayerHPView)

private:
    TextElement*                 _hp{nullptr};
    CharacterHPViewModel::Handle _handle;
};
