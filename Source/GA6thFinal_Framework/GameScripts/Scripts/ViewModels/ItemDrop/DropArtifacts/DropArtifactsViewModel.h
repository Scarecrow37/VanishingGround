#pragma once
#include "ItemDropSystem/Interface/IDropItem.h"

struct DropArtifactsUIData
{
    File::Guid Frame;
    File::Guid Artifact;
    File::Guid Category;
};

class DropArtifactsViewModel final : MVVM::ViewModel<std::vector<DropItemInfo>, std::vector<DropArtifactsUIData>>
{
public:
    explicit DropArtifactsViewModel(MVVM::Model<std::vector<DropItemInfo>>& model);

    std::vector<DropArtifactsUIData> Convert(const std::vector<DropItemInfo>& value) override;

private:
    std::vector<DropArtifactsUIData> _uiDatas;
};
