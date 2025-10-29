#pragma once
#include "RevelationSystem/RevelationElement/RevelationElement.h"

struct RevelationUIData
{
    std::string     Name;
    Color           NameColor;
    File::Guid      Icon;
    RevelationGrade Grade;
    std::string     Description;
};

class RevelationsViewModel final
    : public MVVM::ViewModel<std::vector<std::shared_ptr<RevelationElement>>, std::vector<RevelationUIData>>
{
public:
    explicit RevelationsViewModel(MVVM::Model<std::vector<std::shared_ptr<RevelationElement>>>& model);

protected:
    std::vector<RevelationUIData> Convert(const std::vector<std::shared_ptr<RevelationElement>>& value) override;

private:
    std::vector<RevelationUIData> _uiData;
};