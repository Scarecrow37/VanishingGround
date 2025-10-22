#pragma once
#include "AccessorySystem/AccessoryElement/AccessoryElement.h"

struct AccessoriesUIData
{
    File::Guid Icon;
};

class AccessoriesViewModel final
    : public MVVM::ViewModel<std::vector<AccessoryElement>, std::vector<AccessoriesUIData>>
{
public:
    explicit AccessoriesViewModel(MVVM::Model<std::vector<AccessoryElement>>& model);

protected:
    std::vector<AccessoriesUIData> Convert(const std::vector<AccessoryElement>& value) override;

private:
    std::vector<AccessoriesUIData> _uiData;
};