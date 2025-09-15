#include "pchScripts.h"
#include "DropArtifactsViewModel.h"
#include "ItemDropSystem/Interface/IDropItem.h"
#include "ItemDropSystem/UI/ItemDropUIRootManager.h"

DropArtifactsViewModel::DropArtifactsViewModel(MVVM::Model<std::vector<DropItemInfo>>& model) 
    : 
    ViewModel(model) 
{

}

namespace
{
    void CheckDropArtifactsUIData(const std::string& name, const DropArtifactsUIData& datas)
    {
        if (datas.Category.IsNull())
        {
            std::u8string message = (const char8_t*)name.data();
            message += u8" 카테고리 UI가 존재하지 않습니다.";
            UmLogger.Log(LogLevel::LEVEL_WARNING, message);
        }
        if (datas.Artifact.IsNull())
        {
            std::u8string message = (const char8_t*)name.data();
            message += u8" 유물 UI가 존재하지 않습니다.";
            UmLogger.Log(LogLevel::LEVEL_WARNING, message);
        }
    }
}

std::vector<DropArtifactsUIData> DropArtifactsViewModel::ConvertData(const std::vector<DropItemInfo>& value)
{
    std::vector<DropArtifactsUIData> datas;
    ItemDropUIRootManager*           uiRootManager = SingletonComponent<ItemDropUIRootManager>::GetInstance();
    if (uiRootManager)
    {
        File::Path framePath = (std::string)uiRootManager->ArtifactsUIFrameAsset;
        if (framePath.IsNull())
        {
            UmLogger.Log(LogLevel::LEVEL_WARNING, u8"UI Root Manager에 Artifacts UI FrameAsset을 설정해주세요.");
        }
        for (auto& item : value)
        {
            DropArtifactsUIData data
            {
                .Frame    = framePath.ToGuid(),
                .Artifact = UmFileSystem.GetPathFromAssetID(uiRootManager->GetArtifactIconID(item)).ToGuid(),
                .Category = UmFileSystem.GetPathFromAssetID(DropItemInfo::GetArtifactCategoryAssetID(item.Category)).ToGuid(),
            };
            CheckDropArtifactsUIData(item.Name, data);        
            datas.push_back(data);
        }
    } 
    return datas;
}

std::vector<DropArtifactsUIData> DropArtifactsViewModel::Convert(const std::vector<DropItemInfo>& value)
{
    _uiDatas = ConvertData(value);
    return _uiDatas;
}

