#include "pchScripts.h"
#include "MapPlayerHPViewModel.h"
#include "Utility/SingletonHelper.h"
#include "PlayerSystem/PlayerSystem.h"
#include "Stats/Player/PlayerStatsComponent.h"
#include "Stats/Player/PlayerStats.h"

MapPlayerHPViewModel::MapPlayerHPViewModel(MVVM::Model<std::pair<int, int>>& model)
    : ViewModel(model) 
{

}

PlayerHP MapPlayerHPViewModel::Convert(const std::pair<int, int>& value)
{  
    PlayerHP playerHP{100, 100};
    playerHP.CurrentHP = value.first;
    playerHP.MaxHP     = value.second;
    return playerHP;
}