#pragma once

class Light
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
    ~Light();

public:
    void SetActive(bool isActive) { _isActive = isActive; }
    void SetDirectionalLight(const Vector3& color, const Vector3& ambient, const Vector3& direction,
                             const float& intensity);
    void SetPointLight(const Vector3& color, const Vector3& position, const Vector3& attenuation, const float& range,
                       const float& intensity);
    void SetSpotLight(const Vector3& color, const Vector3& position, const Vector3& direction,
                      const Vector3& attenuation, const float& range, const float& inner, const float& outer,
                      const float& intensity);

public:
    void Update(const float deltaTime);

private:
    LightData _data{};
    Type      _type{};
    bool      _isActive{true};

    const Vector3* _color{nullptr};
    const float*   _intensity{nullptr};
    const Vector3* _float3_1{nullptr};
    const float*   _float_1{nullptr};
    const Vector3* _float3_2{nullptr};
    const float*   _float_2{nullptr};
    const Vector3* _float3_3{nullptr};
    const float*   _float_3{nullptr};
};