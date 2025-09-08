#include "pchScripts.h"
#include "DropArtifactsViewModel.h"
#include "ItemDropSystem/Interface/IDropItem.h"
#include "ItemDropSystem/UI/ItemDropUIRootManager.h"

DropArtifactsViewModel::DropArtifactsViewModel(MVVM::Model<std::vector<DropItemInfo>>& model) 
    : 
    ViewModel(model) 
{

}

std::vector<DropArtifactsUIData> DropArtifactsViewModel::Convert(const std::vector<DropItemInfo>& value)
{
    _uiDatas.clear();
    ItemDropUIRootManager* uiRootManager = ItemDropUIRootManager::GetInstance();
    if (uiRootManager)
    {
        for (auto& item : value)
        {
            File::Path framePath = (std::string)uiRootManager->ArtifactsUIFrameAsset;
            DropArtifactsUIData data
            {
                .ActorPortrait = UmFileSystem.GetPathFromAssetID(item.ID).ToGuid(), 
                .Frame         = framePath.ToGuid(),
            };
            _uiDatas.push_back(data);
        }
    } 
    return _uiDatas;
}

