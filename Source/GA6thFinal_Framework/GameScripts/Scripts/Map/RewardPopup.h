#pragma once
#include "ViewModels/Map/StageFocusViewModel.h"

class Stage;
class RewardPopup : public Component
{
    USING_PROPERTY(RewardPopup)

public:
    RewardPopup();
    ~RewardPopup() override;

public:
    void Awake() override;
    void OnDestroy() override;

public:
    REFLECT_PROPERTY()

protected:
    REFLECT_FIELDS_BEGIN(Component)
    REFLECT_FIELDS_END(RewardPopup)

private:
    void SetupRewardIamge(std::string_view name, int id1, int id2);

private:
    Stage*                      _focusStage = nullptr;
    StageFocusViewModel::Handle _handle;
};
