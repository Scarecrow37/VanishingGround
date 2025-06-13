#pragma once

class Light
{
    friend class RenderScene;    

public:
    enum class TYPE { DIRECTIONAL, POINT, SPOT };

public:
    Light();
    ~Light();

public:
    void SetActive(bool isActive) { _isActive = isActive; }
    void SetPointLight(const Vector3& color, const Vector3& position, const Vector3& attenuation, float range, float intensity);
    void SetDirectionalLight(const Vector3& color, const Vector3& ambient, const Vector3& direction, float intensity);
    void SetSpotLight(const Vector3& color, const Vector3& position, const Vector3& direction, const Vector3& attenuation, float range, float inner, float outer, float intensity);

    void SetColor(const Vector3& color);
    void SetPosition(const Vector3& position);
    void SetDirection(const Vector3& direction);
    void SetIntensity(float intensity);
    void SetRange(float range);
    void SetCone(float inner, float outer);
    void SetAttenuation(float constant, float linear, float quadratic);

private:
    LightData _data;
    TYPE      _type;
    bool      _isActive;
};