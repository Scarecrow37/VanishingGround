#pragma once
#include "UI/Elements/Image/ImageElement.h"

enum class ProgressType : unsigned char
{
    OPACITY,
    LINEAR,
    RADIAL
};

class HoldingProgressImageElement : public ImageElement
{
    USING_PROPERTY(HoldingProgressImageElement)

    using ProgressCompleteCallback = std::function<void()>;

public:
    HoldingProgressImageElement();

protected:
    REFLECT_PROPERTY(Type, IsLeak, HoldDuration, IsHolding, UseMetaTime)

    GETTER(ProgressType, Type) { return ReflectFields->Type; }
    SETTER(ProgressType, Type)
    {
        ReflectFields->Type = value;
        UpdateType();
    }
    PROPERTY(Type)

    GETTER(bool, IsLeak) { return ReflectFields->IsLeak; }
    SETTER(bool, IsLeak) { ReflectFields->IsLeak = value; }
    PROPERTY(IsLeak)

    GETTER(float, HoldDuration) { return ReflectFields->HoldDuration; }
    SETTER(float, HoldDuration) { ReflectFields->HoldDuration = std::max(0.1f, value); }
    PROPERTY(HoldDuration)

    GETTER(bool, IsHolding) { return _isHolding; }
    SETTER(bool, IsHolding)
    {
        _isHolding = value;
    }
    PROPERTY(IsHolding)

    GETTER(bool, UseMetaTime) { return ReflectFields->UseMetaTime; }
    SETTER(bool, UseMetaTime) { ReflectFields->UseMetaTime = value; }
    PROPERTY(UseMetaTime)

public:
    void BindProgressComplete(const ProgressCompleteCallback& callback);

    void Held();

    void ResetProgress();

    void SetElapsedTime(const float t);

protected:
    void Reset() override;
    void Start() override;
    void Update() override;

private:
    void UpdateType();
    void ResetType();

protected:
    REFLECT_FIELDS_BEGIN(ImageElement)
    ProgressType Type         = ProgressType::OPACITY;
    bool         IsLeak       = false;
    bool         UseMetaTime  = false;
    float        HoldDuration = 2.0f;
    REFLECT_FIELDS_END(HoldingProgressImageElement)

private:
    ProgressCompleteCallback _progressCompleteCallback;
    bool                     _isHolding;
    float                    _elapsedTime;
    bool                     _isCompleted;
};