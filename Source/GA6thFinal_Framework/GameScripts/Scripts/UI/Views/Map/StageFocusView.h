#pragma once
#include "ViewModels/Map/StageFocusViewModel.h"

class SmoothScroll;
class StageFocusView : public Component
{
    USING_PROPERTY(StageFocusView)

public:
    StageFocusView();
    ~StageFocusView() override;

public:
    void Awake() override;
    void OnDestroy() override;

public:
    REFLECT_PROPERTY()

protected:
    REFLECT_FIELDS_BEGIN(Component)
    REFLECT_FIELDS_END(StageFocusView)

private:
    SmoothScroll*               _scroll{nullptr};
    SIZE                        _scrollSize{};
    StageFocusViewModel::Handle _handle;
};
