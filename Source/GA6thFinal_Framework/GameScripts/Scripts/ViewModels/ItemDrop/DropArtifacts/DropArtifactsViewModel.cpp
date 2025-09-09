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
                .Frame    = framePath.ToGuid(),
                .Artifact = UmFileSystem.GetPathFromAssetID(item.ID).ToGuid(), 
                .Category = UmFileSystem.GetPathFromAssetID(item.CategoryID).ToGuid(), 
            };
            _uiDatas.push_back(data);
        }
    } 
    return _uiDatas;
}

