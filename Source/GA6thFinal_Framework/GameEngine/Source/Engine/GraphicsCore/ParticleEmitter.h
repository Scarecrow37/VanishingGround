#pragma once
#include "ParticleHelper.h"


class EmitLocator
{
public:
    void            RandomInitialize();
    virtual Vector3 EmitLocate() = 0;
    Vector3         GetFactor() const { return _factor; }
    void            SetFactor(const Vector3 factor) { _factor = factor; }
    std::function<float()>                _randomVal;

protected:
    Vector3                               _factor = {100, 100, 100};
    std::random_device                    _randomizer;
    std::mt19937                          _randomGenerator;
    std::uniform_real_distribution<float> _randomRange;
};

class SphereLocator : public EmitLocator
{
public:
    Vector3 EmitLocate();

private:
    bool _isSphere = true;
};
class CubeLocator : public EmitLocator
{
public:
    Vector3 EmitLocate();

};
class CylinderLocator : public EmitLocator
{
public:
    Vector3 EmitLocate();
};
class ConeLocator : public EmitLocator
{
public:
    Vector3 EmitLocate();
};
class TorusLocator : public EmitLocator
{
public:
    Vector3 EmitLocate();
};
class MeshSurfaceLocator : public EmitLocator
{
public:
    Vector3 EmitLocate();
    void    SetVertices(const std::vector<Vector3>& vertices) { _vertices = vertices; }

private:
    std::vector<Vector3> _vertices;

};

/// <summary>
/// <para>something that is needed for rendering.</para>
/// <para>it can be sprite, mesh, or ribbon.</para>
/// <para>should be filled with infos that has something to do with DX12.</para>
/// <para>for example, textures for sprites, mesh vertices/indices for meshes, tessellation factors for ribbons.</para>
/// </summary>
class ParticleRenderModule
{
public:
};

class SpriteModule : public ParticleRenderModule
{
public:
    void           SetFrameInfo(Vector4 frameInfo);
    void           SetFrameInfo(int widthCount, int heightCount, int startIndex, int totalCount);
    void           LoadAlbedoTexture(std::wstring filePath);
    void           LoadNormalTexture(std::wstring filePath);

    Vector4        GetInitialFrameInfo() const;
    std::shared_ptr<class Texture> GetAlbedoTexture() const;
    std::shared_ptr<class Texture> GetNormalTexture() const;

protected:
    Vector4                        _initialFrameInfo;
    std::shared_ptr<class Texture> _albedoTexture;
    std::shared_ptr<class Texture> _normalTexture;
    std::vector<Vector4>           _preCalculatedFrameInfos;
    void                           CalculateFrameInfos();
    UMPARTICLE_PROPERTY(float, _frameDuration, FrameDuration, 1 / 24.f);



private:
};

class MeshModule : public ParticleRenderModule
{
    
};
class RibbonModule : public ParticleRenderModule
{

};

class ParticleEmitter
{
public:
    /// <summary>
    /// particle rendering type
    /// determine which type to render(ex) sprite, mesh, ribbon)
    /// </summary>
    ParticleType          _particleType;
    ParticleRenderModule* _particleRenderModule;

    /// <summary>
    /// location shape type
    /// determine which shape to emit particle in
    /// </summary>
    LocationShape _locationType;
    EmitLocator*  _emitLocator;
    void          SetLocatorFactor(const Vector3& factor);

    /// <summary>
    /// velocity scale type
    /// determine which type to scale velocity(ex) linear, in cone, from point)
    /// </summary>
    VelocityScaleType _velocityType = VelocityScaleType::LINEAR;
    void              SetVelocityType(VelocityScaleType velType);
    void              SetVelocityFactor(const Vector3& factor);

    void Initialize(SIZE_T maxParticles = 100000, float emissionRate = 500.f, float emitterLifetime = 5.f,
                    LocationShape locatorShape = LocationShape::SPHERE, Vector3 locationFactor = Vector3(1, 1, 1),
                    ParticleType particleType = ParticleType::SPRITE);
    void Update(float deltaTime);
    void UpdateParticleLifeCycle(float deltaTime);

    inline std::vector<class Particle*>& GetParticlePool() { return _particlePool; }

    UINT GetActiveParticleCount() const { return _activeParticleCount; }

protected:
    void InitializeLocator(LocationShape locatorShape, Vector3 factor);

    void AwakeParticle(UINT index);
    UMPARTICLE_PROPERTY_REF(Matrix, _effectWorldMatrix, EffectWorldMatrix, Matrix::Identity);
    Vector3    _emitterPosition;
    Quaternion _emitterRotation;
    UMPARTICLE_PROPERTY(bool, _activeFlag, ActiveFlag, true);
    UMPARTICLE_PROPERTY(float, _emitterAge, EmitterAge, 0.f);
    UMPARTICLE_PROPERTY(float, _emitterLifetime, EmitterLifetime, 5.f);
    UMPARTICLE_PROPERTY(SIZE_T, _maxParticles, MaxParticles, 10000);
    UMPARTICLE_PROPERTY(float, _emissionRate, EmissionRate, 5000.f);
    UMPARTICLE_PROPERTY(bool, _spawnBurstFlag, SpawnBurstFlag, false);
    UMPARTICLE_PROPERTY(float, _spawnBurstCount, SpawnBurstCount, 5000);

    Matrix GetWorldMatrix() const { return _worldMatrix; }

    std::vector<class Particle*> _particlePool;

    void ScaleVelocity();


    std::function<Vector3(void)> _velocityScalingFunciton;

protected :
    float _emissionThreshold;

    // particle pooling
    SIZE_T             _activeParticleCount;
    std::stack<SIZE_T> _inactiveParticleIndices;

    // rotation, translation matrix for scene graph ( manager - system - emitter - particles )

    Matrix _translationMatrix;
    Matrix _rotationMatrix;
    Matrix _worldMatrix;

    // initial value for particles for lerp
    Vector3 _velocityFactor = Vector3(0, 0, 0);
    UMPARTICLE_PROPERTY_REF(Vector3, _velocity, Velocity, Vector3(1, 1, 1));
    UMPARTICLE_PROPERTY_REF(Vector3, _startColor, StartColor, Vector3(1, 1, 1));
    UMPARTICLE_PROPERTY(float, _startOpacity, StartOpacity, 0.f);
    UMPARTICLE_PROPERTY_REF(Vector3, _endColor, EndColor, Vector3(1, 1, 1));
    UMPARTICLE_PROPERTY(float, _endOpacity, EndOpacity, 1.f);
    UMPARTICLE_PROPERTY_REF(Vector4, _startScale, StartScale, Vector4(1, 1, 1, 1));
    UMPARTICLE_PROPERTY_REF(Vector4, _endScale, EndScale, Vector4(1, 1, 1, 1));

    UMPARTICLE_PROPERTY(float, _particleLifetime, ParticleLifetime, 1.f);
    UMPARTICLE_PROPERTY(float, _particleMass, ParticleMass, 0.1f);
    UMPARTICLE_PROPERTY(float, _particleDistributionOffset, ParticleDistributionOffset, 1.f);
};
