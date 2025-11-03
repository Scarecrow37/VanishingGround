#pragma once
class TransformNameTracker : public Component
{
    USING_PROPERTY(TransformNameTracker)

public:
    TransformNameTracker();
    ~TransformNameTracker() override;

public:
    GETTER(const std::string&, Name) { return ReflectFields->Name; }
    SETTER(const std::string&, Name) { ReflectFields->Name = value;; }
    PROPERTY(Name);

    REFLECT_PROPERTY(Name)

protected:
    REFLECT_FIELDS_BEGIN(Component)
    std::string Name;
    REFLECT_FIELDS_END(TransformNameTracker)

    void Awake() override;
};

