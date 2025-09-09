#include "pchScripts.h"
#include "IDropItem.h"
#include "ItemDropSystem/UI/ItemDropUIRootManager.h"

int DropItemInfo::GetArtifactCategoryAssetID(ArtifactDropType type)
{
    if (ItemDropUIRootManager* uiRootManager = ItemDropUIRootManager::GetInstance())
    {
        return uiRootManager->GetArtifactCategoryAssetID(type);
    }
    return 0;
}
