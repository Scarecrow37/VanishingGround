#include "pchScripts.h"
#include "SturnResistanceView.h"
#include "Stats/Player/PlayerStatsComponent.h"
#include "Stats/Player/PlayerStats.h"
#include "UI/Elements/Text/TextElement.h"
#include "PlayerSystem/PlayerSystem.h"

UMREAL_COMPONENT(PlayerSturnResistanceView)

PlayerSturnResistanceView::PlayerSturnResistanceView() = default;
PlayerSturnResistanceView::~PlayerSturnResistanceView() = default;

void PlayerSturnResistanceView::Start() 
{
    _targetText = GetComponent<TextElement>();
    Base::Start();
    if (PlayerSystem* system = SingletonComponent<PlayerSystem>::GetInstance())
    {
        _viewModelHandle = UmWatcher.Watch<SturnResistanceViewModel, int>(PlayerStats::MODEL_STURN_KEY, [this](const int& value) 
        {
            if (_targetText)
            {
                _targetText->Text = std::to_string(value);
            }
        });

        if (auto component = system->GetPlayerStats())
        {
            _targetText->Text = std::to_string(component->GetStats().StunResistance);
        }      
    }
}

void PlayerSturnResistanceView::OnDestroy() 
{
    Base::OnDestroy();
    UmWatcher.Blind<SturnResistanceViewModel>(PlayerStats::MODEL_STURN_KEY, _viewModelHandle);
}
