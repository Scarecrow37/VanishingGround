#pragma once

class UIComponent : public Component
{
    friend class Transform;
    USING_PROPERTY(UIComponent)

public:
    UIComponent();

protected:
    REFLECT_FIELDS_BEGIN(Component)

    REFLECT_FIELDS_END(UIComponent)

    //이 컴포넌트를 소유한 게임 오브젝트에 다른 자식 게임 오브젝트가 추가되었을 때 호출됩니다.
    virtual void OnAttachChild(GameObject* childGameObject) {};

    //이 컴포넌트를 소유한 게임 오브젝트가 다른 부모 게임 오브젝트로부터 분리되었을 때 호출됩니다.
    virtual void OnDetachParent(GameObject* previousParentGameObject) {};
};