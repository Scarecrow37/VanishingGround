#include "pchScripts.h"
#include "IDropItem.h"
#include "ItemDropSystem/UI/ItemDropUIRootManager.h"
#include "Utility/SingletonHelper.h"

int DropItemInfo::GetArtifactCategoryAssetID(ArtifactDropType type)
{
    if (ItemDropUIRootManager* uiRootManager = SingletonComponent<ItemDropUIRootManager>::GetInstance())
    {
        return uiRootManager->GetArtifactCategoryAssetID(type);
    }
    return 0;
}
