#pragma once
class TempTransition : public Component
{
    USING_PROPERTY(TempTransition)

public:
    TempTransition();
    ~TempTransition() override;



public:
    REFLECT_PROPERTY()

protected:
    REFLECT_FIELDS_BEGIN(Component)
    REFLECT_FIELDS_END(TempTransition)

    void Awake() override;

private:
    SingletonObject<TempTransition> _singletonObject{this};
};

