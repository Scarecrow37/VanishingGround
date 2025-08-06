#pragma once

class HpViewModel final : public MVVM::ViewModel<int, float>
{
public:
    explicit HpViewModel(MVVM::Model<int>& model, int maxHp);

protected:
    float Convert(const int& value) override;

private:
    int _maxHp;
};