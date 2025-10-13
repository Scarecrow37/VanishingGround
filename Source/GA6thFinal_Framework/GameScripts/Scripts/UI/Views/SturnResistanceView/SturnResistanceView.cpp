#include "pchScripts.h"
#include "SturnResistanceView.h"
#include "Stats/Player/PlayerStats.h"
#include "UI/Elements/Text/TextElement.h"
#include "PlayerSystem/PlayerSystem.h"

UMREAL_COMPONENT(PlayerSturnResistanceView)

PlayerSturnResistanceView::PlayerSturnResistanceView() = default;
PlayerSturnResistanceView::~PlayerSturnResistanceView() = default;

void PlayerSturnResistanceView::Start() 
{
    Base::Start();
    if (SingletonComponent<PlayerSystem>::GetInstance())
    {
        _viewModelHandle = UmWatcher.Watch<SturnResistanceViewModel, int>(PlayerStats::MODEL_STURN_KEY, [this](const int& value) 
        {
            if (_targetText)
            {
                _targetText->Text = std::to_string(value);
            }
        });
    }
}

void PlayerSturnResistanceView::OnDestroy() 
{
    Base::OnDestroy();
    UmWatcher.Blind<SturnResistanceViewModel>(PlayerStats::MODEL_STURN_KEY, _viewModelHandle);
}
