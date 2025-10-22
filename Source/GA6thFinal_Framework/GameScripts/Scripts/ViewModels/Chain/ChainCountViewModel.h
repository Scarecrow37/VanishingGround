#pragma once

class ChainCountViewModel final : public MVVM::ViewModel<int, int>
{
public:
    explicit ChainCountViewModel(MVVM::Model<int>& model);

protected:
    int Convert(const int& value) override;

};

