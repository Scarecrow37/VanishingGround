#include "pchScripts.h"
#include "SturnResistanceViewModel.h"

SturnResistanceViewModel::SturnResistanceViewModel(MVVM::Model<int>& model) : ViewModel(model) {}

int SturnResistanceViewModel::Convert(const int& value)
{
    return value;
}