#pragma once
#include "ViewModels/Accessories/AccessoriesViewModel.h"

class GridPanel;
class ImageElement;
class AccessoriesView : public Component
{
    USING_PROPERTY(AccessoriesView)

public:
    inline static constexpr const char* VIEW_KEY = "Accessories View";

    AccessoriesView();
    ~AccessoriesView() override;

public:
    REFLECT_PROPERTY()

protected:
    REFLECT_FIELDS_BEGIN(Component)
    REFLECT_FIELDS_END(AccessoriesView)

    void Reset() override;
    void Awake() override;
    void Start() override;
    void OnDestroy() override;

private:
    void FindChildElements();
    void Watch();
    void Blind();

    GridPanel* _accessoriesGridPanel;
    struct UI
    {
        ImageElement* Icon = nullptr;
        ImageElement* Frame = nullptr;
    };
    std::vector<UI> _uiElements;

    AccessoriesViewModel::Handle _handle;
};

