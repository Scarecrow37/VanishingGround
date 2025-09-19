#pragma once

struct PlayerHP
{
    int CurrentHP;
    int MaxHP;
};

class MapPlayerHPViewModel final : public MVVM::ViewModel<std::pair<int, int>, PlayerHP>
{
public:
    explicit MapPlayerHPViewModel(MVVM::Model<std::pair<int, int>>& model);

protected:
    // ViewModel을(를) 통해 상속됨
    PlayerHP Convert(const std::pair<int, int>& value) override;

};