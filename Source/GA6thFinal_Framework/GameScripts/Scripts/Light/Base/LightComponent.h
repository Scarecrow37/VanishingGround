#pragma once

class ILight;
class LightComponent : public Component
{
    USING_PROPERTY(LightComponent)
protected:
    inline static const XMVECTORF32 DEBUG_COLOR = DirectX::Colors::Red;

public:
    REFLECT_PROPERTY(
        LightColor,
        Intensity)

    GETTER(const Color&, LightColor) 
    { 
        return _lightColor;
    }
    SETTER(const Color&, LightColor) 
    {
        _lightColor = value;
        std::memcpy(&ReflectFields->Color[0], & _lightColor.x, sizeof(ReflectFields->Color));
        std::memcpy(&_lightColorVector3.x, & _lightColor.x, sizeof(_lightColorVector3));
    }
    PROPERTY(LightColor)

    const Vector3& GetColor()
    {
        return _lightColorVector3;
    }

    GETTER(float, Intensity)
    {
        return ReflectFields->Intensity;
    }
    SETTER(float, Intensity) 
    { 
        ReflectFields->Intensity = std::max(value, 0.f);
    }
    PROPERTY(Intensity)

    inline const float& GetIntensity()
    {
        return ReflectFields->Intensity;
    }

private:
    GraphicsPointer<ILight> _light;

public:
    LightComponent();
    virtual ~LightComponent() override;
    const GraphicsPointer<ILight>& Lighting;

protected:
    REFLECT_FIELDS_BEGIN(Component)
    float Intensity = 1.f;
    std::array<float, 4> Color{1.f, 1.f, 1.f, 1.f};    
    REFLECT_FIELDS_END(LightComponent)

    /*
    역직렬화 이후 자동으로 호출되는 이벤트 함수 입니다.
    직접 override 해서 사용합니다.
    */
    virtual void DeserializedReflectEvent() override;

    virtual void Reset();

    virtual void OnDrawDebug() override;

private:
    Color _lightColor{1.f, 1.f, 1.f, 1.f};
    Vector3 _lightColorVector3{1.f, 1.f, 1.f};

private:
#ifdef _UMEDITOR
    SceneGizmo _gizmo{this};
#endif
};