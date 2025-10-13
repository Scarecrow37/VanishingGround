#include "pchScripts.h"
#include "RewardPopup.h"
#include "UI/Elements/Image/ImageElement.h"
#include "Stage.h"

UMREAL_COMPONENT(RewardPopup)

RewardPopup::RewardPopup() = default;
RewardPopup::~RewardPopup() = default;

void RewardPopup::Awake()
{    
    try
    {
        _handle = UmWatcher.Watch<StageFocusViewModel, Stage*>("StageFocus", [this](const Stage* value) {
            if (value)
            {
                const auto& dropItems = value->GetDropItems();
                
                SetupRewardIamge("Level 1", dropItems[0], dropItems[1]);
                SetupRewardIamge("Level 2", dropItems[2], dropItems[3]);
                SetupRewardIamge("Level 3", dropItems[4], dropItems[5]);
            }
        });
    }
    catch (const std::exception& e)
    {
        UmLogger.Log(LogLevel::LEVEL_ERROR, "Watch Failed.");
        UmLogger.Log(LogLevel::LEVEL_ERROR, e.what());
    }
}

void RewardPopup::OnDestroy() 
{
    UmWatcher.Blind<StageFocusViewModel>("StageFocus", _handle);
}

void RewardPopup::SetupRewardIamge(std::string_view name, int id1, int id2)
{    
    if (Transform* level = transform->Find(name);  level)
    {
        level->Find("Reward 1")
            ->gameObject->GetComponent<ImageElement>()
            ->SetImage(UmFileSystem.GetGuidFromAssetID(id1));

        level->Find("Reward 2")
            ->gameObject->GetComponent<ImageElement>()
            ->SetImage(UmFileSystem.GetGuidFromAssetID(id2));
    }
}