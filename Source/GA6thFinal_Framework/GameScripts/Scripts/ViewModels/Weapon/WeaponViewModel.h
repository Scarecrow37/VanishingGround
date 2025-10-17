#pragma once
#include "TurnSystem/TurnActor/TurnActor.h"

struct WeaponUIData
{
    bool         Enable;
    std::string  WeaponName;
    Color        GradeColor;
    File::Guid   WeaponIcon;
    std::string  HitDamage;
    std::string  CriticalDamage;
    std::string  Speed;
    std::string  AttackCount;
    std::string  Description;
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