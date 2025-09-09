#pragma once

class SmoothScroll;
class StageFocusView : public Component
{
    USING_PROPERTY(StageFocusView)

public:
    StageFocusView();
    ~StageFocusView() override;

public:
    void Watch(const std::string& key);

public:
    REFLECT_PROPERTY()

protected:
    REFLECT_FIELDS_BEGIN(Component)
    REFLECT_FIELDS_END(StageFocusView)

private:
    SmoothScroll* _scroll{nullptr};
    SIZE          _scrollSize{};
};
