#pragma once

class TokenHUDViewModel final : public MVVM::ViewModel<int, int>
{  
public:
    explicit TokenHUDViewModel(MVVM::Model<int>& model);

protected:
    int Convert(const int& value) override;
};