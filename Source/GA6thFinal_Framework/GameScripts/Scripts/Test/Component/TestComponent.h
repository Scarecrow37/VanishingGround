#pragma once
#include "UmFramework.h"

class TestComponent : public Component
{
    USING_PROPERTY(TestComponent)
public:
    TestComponent();
    virtual ~TestComponent();

public:
    REFLECT_PROPERTY(
        ObjectDrop, 
        ReflectFields->floatVector,
        Work,
        Move,
        ReflectFields->TestDontDestroyOnLoad
        )

    GETTER_ONLY(std::string_view, ObjectDrop)
    { 
        return ReflectFields->objectName;
    }
    PROPERTY(ObjectDrop)

    GETTER(const Vector3&, WorldPosition) { return transform->WorldPosition; }
    SETTER(const Vector3&, WorldPosition) { transform->WorldPosition = value; }
    PROPERTY(WorldPosition)

    GETTER(const Vector3&, LocalPosition) { return transform->LocalPosition; }
    SETTER(const Vector3&, LocalPosition) { transform->LocalPosition = value; }
    PROPERTY(LocalPosition)

    const std::vector<float>& GetfloatVector() const { return ReflectFields->floatVector; }

    GETTER(int, Work) { return ReflectFields->Work; }
    SETTER(int, Work) { ReflectFields->Work = value; }
    PROPERTY(Work)

    GETTER(int, Move) { return ReflectFields->Move; }
    SETTER(int, Move) { ReflectFields->Move = value; }
    PROPERTY(Move)

protected:
    REFLECT_FIELDS_BEGIN(Component)
    bool TestDontDestroyOnLoad = false;
    int testint = 10;
    float floatTest = 1.f;
    std::string testString = "asdas";
    std::array<int, 10> doubleArray{1,2,3,4,5,6,7,8,9, 10};
    std::vector<float> floatVector{};
    std::string         objectName = "";
    std::array<float, 3> testVector3{};
    int Work = 0;
    int Move = 0;
    REFLECT_FIELDS_END(TestComponent)

protected:
    virtual void Reset() override;
    virtual void Awake() override;
    virtual void Start() override;
    virtual void OnEnable() override;
    virtual void OnDisable() override;
    virtual void Update() override;
    virtual void FixedUpdate() override;
    virtual void OnDestroy() override;
    virtual void OnApplicationQuit() override;

    virtual void SerializedReflectEvent() override;
    virtual void DeserializedReflectEvent() override;
};