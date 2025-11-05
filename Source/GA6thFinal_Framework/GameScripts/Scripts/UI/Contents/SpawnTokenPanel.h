#pragma once

class TokenElement;

class SpawnTokenPanel : public UIComponent
{
    USING_PROPERTY(SpawnTokenPanel)

    static constexpr std::array<float, 4> DEFAULT_COLOR = {0.0f, 0.0f, 0.0f, 1.0f};

public:
    SpawnTokenPanel();

public:
    REFLECT_PROPERTY(FilePath, BeginPoint, EndPoint, BeginOpacity, EndOpacity, LifeTime, EasingFunctionType)

    GETTER_ONLY(std::string, FilePath) { return _Guid.ToPath().string(); }
    PROPERTY(FilePath)

    GETTER(POINT, BeginPoint) { return ReflectFields->BeginPoint; }
    SETTER(POINT, BeginPoint) { ReflectFields->BeginPoint = value; }
    PROPERTY(BeginPoint)

    GETTER(POINT, EndPoint) { return ReflectFields->EndPoint; }
    SETTER(POINT, EndPoint) { ReflectFields->EndPoint = value; }
    PROPERTY(EndPoint)

    GETTER(float, BeginOpacity) { return ReflectFields->BeginOpacity; }
    SETTER(float, BeginOpacity) { ReflectFields->BeginOpacity = std::clamp(value, 0.0f, 1.0f); }
    PROPERTY(BeginOpacity)

    GETTER(float, EndOpacity) { return ReflectFields->EndOpacity; }
    SETTER(float, EndOpacity) { ReflectFields->EndOpacity = std::clamp(value, 0.0f, 1.0f); }
    PROPERTY(EndOpacity)

    GETTER(float, LifeTime) { return ReflectFields->LifeTime; }
    SETTER(float, LifeTime) { ReflectFields->LifeTime = std::max(0.1f, value); }
    PROPERTY(LifeTime)

    GETTER(Mathf::EasingFunctionType, EasingFunctionType) { return ReflectFields->EasingFunctionType; }
    SETTER(Mathf::EasingFunctionType, EasingFunctionType) { ReflectFields->EasingFunctionType = value; }
    PROPERTY(EasingFunctionType)

protected:
    SIZE MeasureOverride(SIZE availableSize) override;
    SIZE ArrangeOverride(SIZE finalSize) override;

    void ImGuiDrawPropertysEvent() override;

protected:
    REFLECT_FIELDS_BEGIN(UIComponent)
    std::string                    Guid;
    POINT                          BeginPoint;
    POINT                          EndPoint;
    float                          BeginOpacity       = 0.0f;
    float                          EndOpacity         = 1.0f;
    float                          LifeTime           = 1.0f;
    Mathf::EasingFunctionType      EasingFunctionType = Mathf::EasingFunctionType::QUAD;
    std::unordered_map<int, std::array<float, 4>> TokenColors;
    REFLECT_FIELDS_END(SpawnTokenPanel)

private:
    File::Guid _Guid;

    int _newID = 0;
    std::array<float, 4> _newColor;
};