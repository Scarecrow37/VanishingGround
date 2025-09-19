#include "pchScripts.h"
#include "CharacterHPViewModel.h"
#include "Utility/SingletonHelper.h"
#include "PlayerSystem/PlayerSystem.h"
#include "Stats/Player/PlayerStatsComponent.h"
#include "Stats/Player/PlayerStats.h"

CharacterHPViewModel::CharacterHPViewModel(MVVM::Model<std::pair<int, int>>& model)
    : ViewModel(model) 
{

}

CharacterHP CharacterHPViewModel::Convert(const std::pair<int, int>& value)
{  
    CharacterHP playerHP{100, 100};
    playerHP.CurrentHP = value.first;
    playerHP.MaxHP     = value.second;
    return playerHP;
}