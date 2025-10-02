#include "pch.h"
#include "Particle.h"
#include "ParticleEmitter.h"
#include "Light.h"

ParticleEmitter::ParticleEmitter() = default;
ParticleEmitter::~ParticleEmitter()
{
    _particlePool.clear();

    delete _emitLocator;
    _emitLocator = nullptr;

    delete _particleRenderModule;
    _particleRenderModule = nullptr;

    if (nullptr != _light)
    {
        _light->SetDestroy();
        delete _light;
        _light = nullptr;
    }
}
ParticleEmitter::ParticleEmitter(const ParticleEmitter& other)
    : _particleType(other._particleType), _locationType(other._locationType), _velocityType(other._velocityType),
      _emitterRotationQ(other._emitterRotationQ), _emitterRotationE(other._emitterRotationE),
      _emitterPosition(other._emitterPosition)
{
    _emitterLifetime                 = other.GetEmitterLifetime();
    _maxParticles                    = other.GetMaxParticles();
    _emissionRate                    = other.GetEmissionRate();
    _startDelay                      = other.GetStartDelay();
    _spawnBurstFlag                  = other.GetSpawnBurstFlag();
    _spawnBurstCount                 = other.GetSpawnBurstCount();
    _emitterName                     = other.GetEmitterName();
    _velocityFactor                  = other.GetVelocityFactor();
    _startColor                      = other.GetStartColor();
    _startOpacity                    = other.GetStartOpacity();
    _endColor                        = other.GetEndColor();
    _endOpacity                      = other.GetEndOpacity();
    _startScale                      = other.GetStartScale();
    _endScale                        = other.GetEndScale();
    _particleLifetime                = other.GetParticleLifetime();
    _particleMass                    = other.GetParticleMass();
    _particleStartDistributionOffset = other.GetParticleStartDistributionOffset();
    _particleEndDistributionOffset   = other.GetParticleEndDistributionOffset();
    _dragPoint                       = other.GetDragPoint();
    _dragForce                       = other.GetDragForce();
}
void ParticleEmitter::SetLocatorFactor(const Vector3& factor)
{
    _emitLocator->SetFactor(factor);
}
void ParticleEmitter::SetVelocityType(VelocityScaleType velType)
{
    _velocityType = velType;
}
void ParticleEmitter::Initialize(SIZE_T maxParticles /*= 100000*/, float emissionRate /*= 500.f*/,
                                 float             emitterLifetime /*= 5.f*/,
                                 LocationShape     locatorShape /*= LocationShape::SPHERE*/,
                                 Vector3           locationFactor /*= Vector3(1,1,1)*/,
                                 ParticleType      particleType /*= ParticleType::SPRITE*/,
                                 std::wstring_view meshspritePath /*= L""*/)
{
    _emitterLifetime = emitterLifetime;
    _particleType    = particleType;
    switch (particleType)
    {
    case ParticleType::SPRITE:
        _particleRenderModule = new SpriteModule();
        break;
    case ParticleType::MESH:
        _particleRenderModule = new MeshModule();
        break;
    case ParticleType::RIBBON:
        _particleRenderModule = new RibbonModule();
        break;
    default:
        _particleRenderModule = new SpriteModule();
        break;
    }
    if (meshspritePath == L"")
    {
        _particleRenderModule->SetModelAndTexturePath(L"BlackTexture");
    }
    else
    {
        _particleRenderModule->SetModelAndTexturePath(meshspritePath);
    }

    _locationType = locatorShape;
    switch (_locationType)
    {
    case LocationShape::SPHERE:
        _emitLocator = new SphereLocator();
        break;
    case LocationShape::CUBE:
        _emitLocator = new CubeLocator();
        break;
    case LocationShape::CYLINDER:
        _emitLocator = new CylinderLocator();
        break;
    case LocationShape::TORUS:
        _emitLocator = new TorusLocator();
        break;

    case LocationShape::CONE:
        _emitLocator = new ConeLocator();
        break;
    case LocationShape::MESH_SURFACE:
        _emitLocator = new MeshSurfaceLocator();
        break;
    default:
        _emitLocator = new SphereLocator();
        break;
    }
    _emitLocator->RandomInitialize();
    _emitLocator->SetFactor(locationFactor);
    _maxParticles = maxParticles;
    _emissionRate = emissionRate;
    _particlePool.resize(_maxParticles);
}
void ParticleEmitter::Update(float deltaTime)
{
    if (false == _delayFlag)
    {

        _delayTimer += deltaTime;
        if (_delayTimer >= _startDelay)
        {
            _activeFlag = true;
            _delayFlag  = true;
        }
        else
        {
            return;
        }
    }

    _emitterAge += deltaTime;
    if (_emitterAge >= _emitterLifetime - _particleLifetime)
    {
        _endFlag = true;
        // return;
    }
    if (_emitterAge >= _emitterLifetime)
    {
        _activeFlag = false;
        _emitterAge = 0;
        return;
    }

    _translationMatrix = Matrix::CreateTranslation(_emitterPosition);
    _rotationMatrix    = Matrix::CreateFromQuaternion(_emitterRotationQ);
    _worldMatrix       = _rotationMatrix * _translationMatrix * _effectWorldMatrix;
    _finalPos          = _worldMatrix.Translation();
    float value        = _emissionRate * _particleLifetime;
    if (true == _useLight)
    {
        if (value > 0)
            _lightCurrentIntensity = (float)std::lerp(0, _lightIntensity, _activeParticleCount / value);
        _lightCurrentRange = _lightRange;
    }
}
void ParticleEmitter::UpdateParticleLifeCycle(float deltaTime)
{
    // 수명 다한 파티클 비활성화
    for (int i = 0; i < _activeParticleCount; ++i)
    {
        _particlePool[i].SetAge(_particlePool[i].GetAge() + deltaTime);
        if (_particlePool[i].GetAge() >= _particleLifetime)
        {
            _activeParticleCount--;
            std::swap(_particlePool[i], _particlePool[_activeParticleCount]);
            i--;
        }
    }
    if (true == _endFlag)
    {
        if (_activeParticleCount == 0)
        {
            _activeFlag = false;
        }
        return;
    }

    // 새 파티클 생성
    size_t newParticles = 0;
    _emissionThreshold += deltaTime * _emissionRate;
    if (true == _spawnBurstFlag && false == _isSpawnBursted)
    {
        _emissionThreshold += _spawnBurstCount;
        _isSpawnBursted = true;
    }
    if (_emissionThreshold >= 1)
    {
        newParticles = static_cast<size_t>(_emissionThreshold);
        _emissionThreshold -= newParticles;
    }

    size_t availableSlots = _maxParticles - _activeParticleCount;
    if (newParticles > availableSlots)
    {
        newParticles = availableSlots;
    }

    for (size_t i = 0; i < newParticles; ++i)
    {
        AwakeParticle(static_cast<UINT>(_activeParticleCount));
        _activeParticleCount++;
    }
}
void ParticleEmitter::FlushTextureResource()
{
    if (ParticleType::SPRITE == _particleType)
    {
        SpriteModule* spritemodule = static_cast<SpriteModule*>(_particleRenderModule);
        if (true == spritemodule->GetTextureChangeFlag())
        {
            spritemodule->SetTextureChangeFlag(false);
            spritemodule->SetAlbedoTexture(
                Global::resourceManager->LoadResource<Texture>(spritemodule->GetModelAndTexturePath()));
            Global::particleManager->RefreshEditor();
        }
    }
    if (ParticleType::RIBBON == _particleType)
    {
        RibbonModule* ribbonmodule = static_cast<RibbonModule*>(_particleRenderModule);
        if (true == ribbonmodule->GetTextureChangeFlag())
        {
            ribbonmodule->SetTextureChangeFlag(false);
            ribbonmodule->SetAlbedoTexture(
                Global::resourceManager->LoadResource<Texture>(ribbonmodule->GetModelAndTexturePath()));
            Global::particleManager->RefreshEditor();
        }
    }
}
void ParticleEmitter::Reset()
{
    _delayFlag = _activeFlag = false;
    _isSpawnBursted          = false;
    _endFlag                 = false;
    _delayTimer              = 0.f;
    _emitterAge              = 0.f;
    _activeParticleCount     = 0;
    _emissionThreshold       = 0;
}
void ParticleEmitter::SetEmitterRotationQ(const Quaternion& value)
{
    _emitterRotationQ = value;
    _emitterRotationE = _emitterRotationQ.ToEuler();
}
void ParticleEmitter::SetEmitterRotationE(const Vector3& value)
{
    _emitterRotationE = value;
    _emitterRotationQ = Quaternion::CreateFromYawPitchRoll(_emitterRotationE);
}
void ParticleEmitter::InitializeLocator(LocationShape locatorShape, Vector3 factor)
{
    switch (locatorShape)
    {
    case LocationShape::SPHERE:
        _emitLocator = new SphereLocator();
        break;
    case LocationShape::CUBE:
        _emitLocator = new CubeLocator();
        break;
    case LocationShape::CYLINDER:
        _emitLocator = new CylinderLocator();
        break;
    case LocationShape::CONE:
        _emitLocator = new ConeLocator();
        break;
    case LocationShape::TORUS:
        _emitLocator = new TorusLocator();
        break;
    case LocationShape::MESH_SURFACE:
        _emitLocator = new MeshSurfaceLocator();
        break;
    }
    _emitLocator->SetFactor(factor);
    _emitLocator->RandomInitialize();
}
void ParticleEmitter::AwakeParticle(UINT index)
{
    Vector3 offset = {_emitLocator->_randomVal(), _emitLocator->_randomVal(), _emitLocator->_randomVal()};
    Vector4 location = {1, 1, 1, 1};
    Vector3 tempPos  = _emitLocator->EmitLocate();
    float   ratio    = 0;
    if (_emitterLifetime <= _particleLifetime)
        ratio = _emitterAge / _emitterLifetime;
    else
        ratio = _emitterAge / (_emitterLifetime - _particleLifetime);

    Vector3 currentOffset = Vector3::Lerp(_particleStartDistributionOffset, _particleEndDistributionOffset, ratio);

    location.x = tempPos.x + offset.x * (0 < currentOffset.x ? currentOffset.x : 0);
    location.y = tempPos.y + offset.y * (0 < currentOffset.y ? currentOffset.y : 0);
    location.z = tempPos.z + offset.z * (0 < currentOffset.z ? currentOffset.z : 0);

    if (_useWorldSpace)
    {
        location = Vector4::Transform(location, _worldMatrix);
    }

    _particlePool[index].SetPosition(location);
    ScaleVelocity({location.x, location.y, location.z});
    Vector3 finalVelocity = _velocity;
    finalVelocity         = Vector3::TransformNormal(_velocity, _worldMatrix);
    _particlePool[index].SetVelocity(finalVelocity);
    _particlePool[index].SetAxis(GetScaleByVelocityFlag() ? finalVelocity : _particleAxis);
    _particlePool[index].SetAge(0.f);
    _particlePool[index].SetMass(_particleMass);

    if (ParticleType::SPRITE == _particleType)
    {
        auto    spritemodule = static_cast<SpriteModule*>(_particleRenderModule);
        Vector4 frameInfo    = {spritemodule->GetFrameDuration(), 0, 0, 0};
        _particlePool[index].SetFrameinfo(frameInfo);
    }
    _particlePool[index].SetInitialMatrix(_worldMatrix.Transpose());
}
void ParticleEmitter::ScaleVelocity(Vector3 pos)
{
    switch (_velocityType)
    {
    case VelocityScaleType::LINEAR:
        _velocity = _velocityFactor;
        break;
    case VelocityScaleType::CONE:

    case VelocityScaleType::POINT:
        ScaleVelFromPoint(pos);
        break;
    case VelocityScaleType::CUSTOM:
        _velocity = _velocityScalingFunciton();
        break;
    default:
        _velocity = _velocityFactor;
        break;
    }
}
void ParticleEmitter::ScaleVelFromPoint(Vector3 pos)
{
    Vector4 vel       = {pos.x, pos.y, pos.z, 0};
    Vector3 direction = {vel.x, vel.y, vel.z};
    direction.Normalize();
    _velocity = direction * _velocityFactor.x;
}
void ParticleEmitter::ScaleVelInCone(Vector3 pos) {}
void ParticleEmitter::InitializeLight(std::string_view scenenName)
{
    _light = new Light();
    _light->SetPointLight(_lightColor, _finalPos, _lightAttenuation, _lightCurrentRange, _lightCurrentIntensity);
    _light->SetActive(&_activeFlag);

    Global::lightCore->RegisterLight(scenenName, _light);
    if (scenenName == "Game")
    {
        Global::lightCore->RegisterLight("Editor", _light);
    }
}
void ParticleEmitter::SetLightFlag(bool value) 
{
    _useLight = value;
    if (value)
        _light->SetActive(&_activeFlag);
    else if (_light)
        _light->SetActive(&_useLight);
}
