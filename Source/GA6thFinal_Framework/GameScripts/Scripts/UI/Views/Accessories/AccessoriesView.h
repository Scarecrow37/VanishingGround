#pragma once
#include "ViewModels/Accessories/AccessoriesViewModel.h"

class GridPanel;
class ImageElement;
class DescriptionPanel;
class AccessoriesView : public Component
{
    USING_PROPERTY(AccessoriesView)

public:
    inline static constexpr const char* VIEW_KEY = "Accessories View";
    struct UI
    {
        ImageElement* Icon  = nullptr;
        ImageElement* Frame = nullptr;
        ImageElement* Focus = nullptr;
    };

    AccessoriesView();
    ~AccessoriesView() override;

public:
    REFLECT_PROPERTY()

    GETTER_ONLY(GridPanel*, AccessoriesGridPanel) { return _accessoriesGridPanel; }
    PROPERTY(AccessoriesGridPanel)

    GETTER_ONLY(const std::vector<AccessoriesView::UI>&, UIElements) { return _uiElements; }
    PROPERTY(UIElements)

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
    std::vector<UI> _uiElements;

    AccessoriesViewModel::Handle _handle;

private:
    void AddCallback();
    void ClearCallback();

    std::vector<std::pair<UmDelegate<>*, UmDelegate<>::Handle>> _callbacks;

    void FocusIn(size_t index);
    void FocusOut(size_t index);
    void ShowTooltip(size_t index);
    void HideTooltip(size_t index);

private:
    std::weak_ptr<GameObject>       _focusInfoUIObject;
    std::weak_ptr<DescriptionPanel> _focusInfoDescription;
};

