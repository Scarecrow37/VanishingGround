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
    ItemDropUIRootManager* uiRootManager = SingletonComponent<ItemDropUIRootManager>::GetInstance();
    if (uiRootManager)
    {
        for (auto& item : value)
        {
            DropArtifactsUIData data
            {
                .Artifact = UmFileSystem.GetGuidFromAssetID(DropItemInfo::GetArtifactIconID(item)),
                .Category = UmFileSystem.GetGuidFromAssetID(DropItemInfo::GetArtifactCategoryAssetID(item.Category, false)),
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

