#include "pchScripts.h"
#include "StageFocusViewModel.h"
//#include "Map/Stage.h"

StageFocusViewModel::StageFocusViewModel(MVVM::Model<Stage*>& model)
    : ViewModel(model) {}

Stage* StageFocusViewModel::Convert(Stage* const& value)
{    
    return value;
}
