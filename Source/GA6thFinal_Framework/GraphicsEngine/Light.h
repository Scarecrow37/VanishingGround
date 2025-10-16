#pragma once
#include "GraphicsBase.h"
#include "Interface/ILight.h"

class Light : public GraphicsBase, public ILight
{
public:
    enum class Type
    {
        NONE,
        DIRECTIONAL,
        POINT,
        SPOT,
        SHADOWPOINT
    };

public:
    Light();
    virtual ~Light();

public:
    bool             IsActive() const override;
    Type             GetType() const { return _type; }
    const LightData& GetLightData() const { return _data; }

public:
    void SetActive(const bool* isActive) override;
    void SetDirectionalLight(const Vector3& color, const Vector3& ambient, const Vector3& direction, const float& intensity) override;
    void SetPointLight(const Vector3& color, const Vector3& position, const Vector3& attenuation, const float& range, const float& intensity) override;
    void SetSpotLight(const Vector3& color, const Vector3& position, const Vector3& direction, const Vector3& attenuation, const float& range, const float& inner, const float& outer, const float& intensity) override;
    void SetShadowPointLight(const Vector3& color, const Vector3& position, const Vector3& attenuation, const float& range, const float& intensity) override;

public:
    void AddReference() override;
    void Release() override;

public:
    void Update(const float deltaTime);

private:
    LightData _data{};
    Type      _type{};

    const Vector3* _color{nullptr};
    const float*   _intensity{nullptr};
    const Vector3* _float3_1{nullptr};
    const float*   _float_1{nullptr};
    const Vector3* _float3_2{nullptr};
    const float*   _float_2{nullptr};
    const Vector3* _float3_3{nullptr};
    const float*   _float_3{nullptr};
};