#pragma once

class SturnResistanceViewModel final : public MVVM::ViewModel<int, int>
{
public:
    explicit SturnResistanceViewModel(MVVM::Model<int>& model);

protected:
    int Convert(const int& value) override;
};
