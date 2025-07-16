#pragma once
#include "GraphicsBase.h"

class Light : public GraphicsBase
{
    friend class RenderScene;

public:
    enum class Type
    {
        NONE,
        DIRECTIONAL,
        POINT,
        SPOT
    };

public:
    Light();
    virtual ~Light();

public:
    void RegisterComponent(std::string_view sceneName) override;
    void RegisterComponent() override;

public:
    void SetDirectionalLight(const Vector3& color, const Vector3& ambient, const Vector3& direction, const float& intensity);
    void SetPointLight(const Vector3& color, const Vector3& position, const Vector3& attenuation, const float& range, const float& intensity);
    void SetSpotLight(const Vector3& color, const Vector3& position, const Vector3& direction, const Vector3& attenuation, const float& range, const float& inner, const float& outer, const float& intensity);

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