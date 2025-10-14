#pragma once

class UINaviKeyBindHelper : public Component
{
    USING_PROPERTY(UINaviKeyBindHelper)

public:
    enum class BindType
    {
        LEFT,
        RIGHT,
        UP,
        DOWN
    };
    static NavigationKey MakeDpadKey(BindType type);
    static NavigationKey MakeThumbStickKey(BindType type);

    UINaviKeyBindHelper();
    ~UINaviKeyBindHelper() override;

public:
    void FindNavi();
    void BindNavi(UINavigationComponent* navi, BindType type, int navigationID);

    REFLECT_PROPERTY()
    
protected:
    REFLECT_FIELDS_BEGIN(Component)
    REFLECT_FIELDS_END(UINaviKeyBindHelper)

    void Added() override;
    void ImGuiDrawPropertysEvent() override;

private:
    std::weak_ptr<Component> _navi;
};

