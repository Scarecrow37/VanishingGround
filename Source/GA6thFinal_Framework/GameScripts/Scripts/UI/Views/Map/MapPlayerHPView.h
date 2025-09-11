#pragma once
#include "ViewModels/Map/MapPlayerHPViewModel.h"

class TextElement;
class MapPlayerHPView : public Component
{
    USING_PROPERTY(MapPlayerHPView)

public:
    MapPlayerHPView();
    ~MapPlayerHPView() override;

public:
    void Awake() override;

public:
    REFLECT_PROPERTY()

protected:
    REFLECT_FIELDS_BEGIN(Component)
    REFLECT_FIELDS_END(MapPlayerHPView)

private:
    TextElement*                 _hp{nullptr};
    MapPlayerHPViewModel::Handle _handle;
};
