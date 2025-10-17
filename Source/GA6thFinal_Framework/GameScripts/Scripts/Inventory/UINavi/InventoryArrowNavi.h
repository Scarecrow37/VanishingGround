#pragma once

class HorizontalPageUIManager;
class InventoryArrowNavi : public UINavigationComponent
{
    USING_PROPERTY(InventoryArrowNavi)
public:
    InventoryArrowNavi();
    ~InventoryArrowNavi() override;

    enum class ArrowDir
    {
        LEFT,
        RIGHT
    };

public:
    void SetScrollManager(HorizontalPageUIManager* manager);

    GETTER(ArrowDir, DIR) { return ReflectFields->DIR; }
    SETTER(ArrowDir, DIR) { ReflectFields->DIR = value; }
    // 화살표 방향
    // type : InventoryArrowNavi::ArrowDir
    PROPERTY(DIR)

    REFLECT_PROPERTY(DIR)

protected:
    REFLECT_FIELDS_BEGIN(UINavigationComponent)
    ArrowDir DIR = ArrowDir::LEFT;
    REFLECT_FIELDS_END(InventoryArrowNavi)

    /// <summary>
    /// 포커스가 들어올 때 호출되는 함수입니다.
    /// </summary>
    void FocusIn(FocusCallType callType) override;

    std::weak_ptr<Component> _scrollManager;
};

