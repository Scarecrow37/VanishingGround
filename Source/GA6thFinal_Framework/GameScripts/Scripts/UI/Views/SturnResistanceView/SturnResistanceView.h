#pragma once
#include "ViewModels/SturnResistance/SturnResistanceViewModel.h"

class TextElement;
/*이 컴포넌트가 부착된 오브젝트에 Text Element가 존재해야합니다.*/
class PlayerSturnResistanceView : public Component
{
    USING_PROPERTY(PlayerSturnResistanceView)

public:
    PlayerSturnResistanceView();
    ~PlayerSturnResistanceView() override;

public:
    REFLECT_PROPERTY()

protected:
    REFLECT_FIELDS_BEGIN(Component)
    REFLECT_FIELDS_END(PlayerSturnResistanceView)

    void Start() override;
    void OnDestroy() override;

    SturnResistanceViewModel::Handle _viewModelHandle;
    TextElement* _targetText = nullptr;
};

