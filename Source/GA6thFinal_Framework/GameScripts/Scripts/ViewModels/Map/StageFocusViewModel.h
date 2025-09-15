#pragma once

class Stage;
class StageFocusViewModel final : public MVVM::ViewModel<Stage*, Stage*>
{
public:
    explicit StageFocusViewModel(MVVM::Model<Stage*>& model);

protected:
    // ViewModel을(를) 통해 상속됨
    Stage* Convert(Stage* const& value) override;
};