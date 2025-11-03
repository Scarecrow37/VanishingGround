#include "pchScripts.h"
#include "TokenHUDViewModel.h"

TokenHUDViewModel::TokenHUDViewModel(MVVM::Model<int>& model)
    : MVVM::ViewModel<int, int>(model)
{
}

int TokenHUDViewModel::Convert(const int& value)
{
    return value;
}