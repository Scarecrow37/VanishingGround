#pragma once

struct PlayerHP
{
    int CurrentHP;
    int MaxHP;
};

class MapPlayerHPViewModel final : public MVVM::ViewModel<int, PlayerHP>
{
public:
    explicit MapPlayerHPViewModel(MVVM::Model<int>& model, int maxHP);

protected:
    // ViewModel을(를) 통해 상속됨
    PlayerHP Convert(const int& value) override;

private:
    int _maxHP;
};