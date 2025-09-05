#pragma once

class IDropItem;

struct DropArtifactsUIData
{
    File::Guid ActorPortrait;
    File::Guid Frame;
};

class DropArtifactsViewModel final : MVVM::ViewModel<std::vector<IDropItem*>, std::vector<DropArtifactsUIData>>
{
public:
    explicit DropArtifactsViewModel(MVVM::Model<std::vector<IDropItem*>>& model);

    std::vector<DropArtifactsUIData> Convert(const std::vector<IDropItem*>& value) override;

private:
    std::vector<DropArtifactsUIData> _uiDatas;
};
