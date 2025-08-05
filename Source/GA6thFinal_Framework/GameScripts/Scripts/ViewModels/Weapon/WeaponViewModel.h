#pragma once
#include "TurnSystem/TurnActor/TurnActor.h"

struct WeaponUIData
{
    bool          Enable;
    std::string   WeaponName;
    File::GuidRef WeaponIcon;
    int           HitDamage;
    int           CriticalDamage;
    int           Speed;
    int           AttackCount;
    std::wstring   Description1;
    std::wstring   Description2;
    File::GuidRef Background;
};

class WeaponViewModel final : public MVVM::ViewModel<TurnActor*, WeaponUIData>
{
public:
    explicit WeaponViewModel(MVVM::Model<TurnActor*>& model);

protected:
    WeaponUIData Convert(TurnActor* const& value) override;

private:
    std::vector<WeaponUIData> _turnQueueData;
};