#include "pchScripts.h"
#include "RewardPopup.h"
#include "UI/Elements/Image/ImageElement.h"
#include "ViewModels/Map/StageFocusViewModel.h"
#include "Map/Stage.h"

RewardPopup::RewardPopup() = default;
RewardPopup::~RewardPopup() = default;

void RewardPopup::SetFocusStage(Stage* stage)
{

}

void RewardPopup::Awake()
{
    //UmWatcher.Blind<StageFocusViewModel>("StageFocus");
    //try
    //{
    //    UmWatcher.Watch<StageFocusViewModel, Stage*>("StageFocus", [this](const Stage* value) {
    //        if (value)
    //        {
    //            const auto& dropItems = value->GetDropItems();

    //            // TODO ItemID 기능이 나온 이후 수정
    //            // SetupRewardIamge("Level 1", dropItems[0], dropItems[1]);
    //            // SetupRewardIamge("Level 2", dropItems[2], dropItems[3]);
    //            // SetupRewardIamge("Level 3", dropItems[4], dropItems[5]);

    //            std::uniform_int_distribution<int> dist(101020, 101025);
    //            std::mt19937                       rng(static_cast<unsigned int>(std::chrono::steady_clock::now().time_since_epoch().count()));

    //            SetupRewardIamge("Level 1", dist(rng), dist(rng));
    //            SetupRewardIamge("Level 2", dist(rng), dist(rng));
    //            SetupRewardIamge("Level 3", dist(rng), dist(rng));
    //        }
    //    });
    //}
    //catch (const std::exception& e)
    //{
    //    UmLogger.Log(LogLevel::LEVEL_ERROR, "Watch Failed.");
    //    UmLogger.Log(LogLevel::LEVEL_ERROR, e.what());
    //}
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