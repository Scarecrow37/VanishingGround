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
        emitFactor
        )

    GETTER_ONLY(std::string_view, ObjectDrop)
    { 
        return ReflectFields->objectName;
    }
    PROPERTY(ObjectDrop)

    GETTER(Vector3, emitFactor) 
    { 
        return emit.GetFactor(); 
    }
    SETTER(Vector3, emitFactor)
    { 
        emit.SetFactor(value);
    }
    PROPERTY(emitFactor)

    const std::vector<float>& GetfloatVector() const { return ReflectFields->floatVector; }

private:
    class EmitLocator
    {
    public:
        Vector3         GetFactor() const { return _factor; }
        void            SetFactor(const Vector3 factor) { _factor = factor; }

    protected:
        Vector3                               _factor;
    }
    emit;

protected:
    REFLECT_FIELDS_BEGIN(Component)
    int testint = 10;
    float floatTest = 1.f;
    std::string testString = "asdas";
    std::array<int, 10> doubleArray{1,2,3,4,5,6,7,8,9, 10};
    std::vector<float> floatVector{};
    std::string         objectName = "";
    std::array<float, 3> _factor;
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