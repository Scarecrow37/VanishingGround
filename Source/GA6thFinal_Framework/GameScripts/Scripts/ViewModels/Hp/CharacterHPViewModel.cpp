#include "pchScripts.h"
#include "CharacterHPViewModel.h"

CharacterHPViewModel::CharacterHPViewModel(MVVM::Model<std::pair<int, int>>& model)
    : ViewModel(model) 
{

}

CharacterHP CharacterHPViewModel::Convert(const std::pair<int, int>& value)
{  
    CharacterHP playerHP{value.first, value.second};
    return playerHP;
}