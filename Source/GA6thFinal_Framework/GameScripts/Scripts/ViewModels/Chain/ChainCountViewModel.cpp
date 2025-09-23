#include "pchScripts.h"
#include "ChainCountViewModel.h"

ChainCountViewModel::ChainCountViewModel(MVVM::Model<int>& model) 
: ViewModel(model) 
{

}

int ChainCountViewModel::Convert(const int& value)
{
    return value;
}