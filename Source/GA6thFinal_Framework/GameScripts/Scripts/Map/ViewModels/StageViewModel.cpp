#include "pchScripts.h"
#include "StageViewModel.h"

StageViewModel::StageViewModel(MVVM::Model<bool>& model, const File::Guid& enableGUID, const File::Guid& disableGUID)
    : ViewModel(model)
    , _enableImage(enableGUID)
    , _disableImage(disableGUID)
{
}

File::Guid StageViewModel::Convert(const bool& value)
{
    return value ? _enableImage : _disableImage;
}
