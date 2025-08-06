#include "pchScripts.h"
#include "HpViewModel.h"

HpViewModel::HpViewModel(MVVM::Model<int>& model, const int maxHp): ViewModel(model), _maxHp(maxHp)
{
}

float HpViewModel::Convert(const int& value)
{
    return static_cast<float>(value) / static_cast<float>(_maxHp);
}