#include "pchScripts.h"
#include "MapPlayerHPViewModel.h"

MapPlayerHPViewModel::MapPlayerHPViewModel(MVVM::Model<int>& model, int maxHP)
    : ViewModel(model) 
    , _maxHP(maxHP)
{}

PlayerHP MapPlayerHPViewModel::Convert(const int& value)
{
    PlayerHP playerHP;
    playerHP.CurrentHP = value;
    playerHP.MaxHP = _maxHP;

    return playerHP;
}