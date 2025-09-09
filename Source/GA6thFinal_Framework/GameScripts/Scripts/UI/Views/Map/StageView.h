#pragma once

class StageView : public Component
{
    USING_PROPERTY(StageView)

public:
    StageView();
    ~StageView() override;

public:
    void Watch(const std::string& key);

public:
    REFLECT_PROPERTY()

protected:
    REFLECT_FIELDS_BEGIN(Component)
    REFLECT_FIELDS_END(StageView)
};
