#pragma once

struct CharacterHP
{
    int CurrentHP;
    int MaxHP;
};

class CharacterHPViewModel final : public MVVM::ViewModel<std::pair<int, int>, CharacterHP>
{
public:
    explicit CharacterHPViewModel(MVVM::Model<std::pair<int, int>>& model);

protected:
    // ViewModel을(를) 통해 상속됨
    CharacterHP Convert(const std::pair<int, int>& value) override;
};