#pragma once

class InventoryArrowNavi;
class ImageElement;
class HorizontalPanel;
class HorizontalPageUIManager : public Component
{
    USING_PROPERTY(HorizontalPageUIManager)

public:
    inline static constexpr const char* BAR_FRAME_GUID = "fe989452-79aa-4359-bc82-6710aff6db8d";
    inline static constexpr const char* BAR_FOCUS_GUID = "7a41eb96-e620-4715-b1bc-5bebb7a694f4";

    HorizontalPageUIManager();
    ~HorizontalPageUIManager() override;

public:
    void UpdateHorizontalUI(size_t count);
    void SetHorizontalFocus(size_t index);
    void HorizontalScrollLeft();
    void HorizontalScrollRight();

    size_t ScrollSize() const { return _horizontalBar.size(); }

    enum class DIR
    {
        UNKOWN,
        LEFT,
        RIGHT
    };
    DIR GetLastDIR() const { return _lastDIR; }

public:
    GETTER_ONLY(size_t, CurrentFocus)
    {
        return _currentFocus;
    }
    // type : size_t
    PROPERTY(CurrentFocus)

    REFLECT_PROPERTY(CurrentFocus)

protected:
    REFLECT_FIELDS_BEGIN(Component)
    REFLECT_FIELDS_END(HorizontalPageUIManager)

    void ImGuiDrawPropertysEvent() override; 

    void Awake() override;

private:
    void FindUIElements();

    struct ArrowElement
    {
        ImageElement*       Left      = nullptr;
        InventoryArrowNavi* LeftNavi  = nullptr;
        void                SetEnableLeft(bool enable);

        ImageElement*       Right     = nullptr;
        InventoryArrowNavi* RightNavi = nullptr;
        void                SetEnableRight(bool enable);
    };
    ArrowElement _arrow;

    struct HorizontalBarElement
    {
        ImageElement* Frame = nullptr;
        ImageElement* Focus = nullptr;
    };
    HorizontalPanel*                  _horizontalPanel = nullptr;
    std::vector<HorizontalBarElement> _horizontalBar;

private:
    HorizontalBarElement CreateHorizontalElement();
    void                 UpdateArrow(size_t index);

    size_t _currentFocus = std::numeric_limits<size_t>::max();
    DIR    _lastDIR      = DIR::UNKOWN;
};

