#pragma once

enum class LocationShape
{
    SPHERE,
    CUBE,
    CYLINDER,
    CONE,
    TORUS,
    MESH_SURFACE
};
enum class ParticleType
{
    SPRITE,
    MESH,
    RIBBON
};
class EmitLocator
{
public:
    void            RandomInitialize();
    virtual Vector3 EmitLocate() = 0;
    Vector3         GetFactor() const { return _factor; }
    void            SetFactor(const Vector3 factor) { _factor = factor; }

protected:
    Vector3                               _factor;
    std::random_device                    _randomizer;
    std::mt19937                          _randomGenerator;
    std::uniform_real_distribution<float> _randomRange;
    std::function<float()>                _randomVal;
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
protected:
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

    void Initialize(SIZE_T maxParticles = 100000, float emissionRate = 500.f, float emitterLifetime = 5.f,
                    LocationShape locatorShape = LocationShape::SPHERE, Vector3 locationFactor = Vector3(1,1,1));
    void Update(float deltaTime);

    void SetLocatorFactor(const Vector3& factor);

protected:
    void InitializeLocator(LocationShape locatorShape, Vector3 factor);


protected:
    EmitLocator* _emitLocator;
    SIZE_T       _maxParticles;
    float        _emissionRate;
    float        _emissionThreshold;
    bool         _emitterActiveFlag;
    float        _emitterAge;
    float        _emitterLifetime;
    bool         _isSpawnBursting;
    float        _spawnBurstCount;

    // particle pooling
    class ParticleEffect*       _parentEffect;
    std::vector<class Particle> _particlePool;
    SIZE_T                      _activeParticleCount;
    std::stack<SIZE_T>          _inactiveParticleIndices;

    // rotation, translation matrix for scene graph ( manager - system - emitter - particles )
    Vector3    _emitterPosition;
    Quaternion _emitterRotation;
    Matrix     _translationMatrix;
    Matrix     _rotationMatrix;
    Matrix     _worldMatrix;

    // initial value for particles for lerp
    Vector3 _startVelocity;
    Vector3 _endVelocity;
    Vector4 _startColor;
    Vector4 _endColor;
    Vector3 _startScale;
    Vector3 _endScale;




};
