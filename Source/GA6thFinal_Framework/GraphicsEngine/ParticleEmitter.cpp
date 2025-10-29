#include "pch.h"
#include "Particle.h"
#include "ParticleEmitter.h"
#include "Light.h"

ParticleEmitter::ParticleEmitter()  = default;
ParticleEmitter::~ParticleEmitter() = default; 

void ParticleEmitter::SetLocatorFactor(const Vector3& factor)
{
    if (_emitLocator)
        _emitLocator->SetFactor(factor);
}

void ParticleEmitter::SetVelocityType(VelocityScaleType velType)
{
    _velocityType = velType;
}

void ParticleEmitter::Initialize(SIZE_T maxParticles, float emissionRate, float emitterLifetime,
                                 LocationShape locatorShape, Vector3 locationFactor, ParticleType particleType,
                                 const std::wstring& meshspritePath) // view -> wstring (값 복사로 수명 보장)
{
    _emitterLifetime = emitterLifetime;
    _particleType    = particleType;

    // 렌더 모듈 소유 생성
    switch (particleType)
    {
    case ParticleType::SPRITE:
        _particleRenderModule = std::make_unique<SpriteModule>();
        break;
    case ParticleType::MESH:
        _particleRenderModule = std::make_unique<MeshModule>();
        break;
    case ParticleType::RIBBON:
        _particleRenderModule = std::make_unique<RibbonModule>();
        break;
    default:
        _particleRenderModule = std::make_unique<SpriteModule>();
        break;
    }

    if (meshspritePath.empty())
    {
        _particleRenderModule->SetModelAndTexturePath(L"BlackTexture");
    }
    else
    {
        _particleRenderModule->SetModelAndTexturePath(meshspritePath);
    }

    // 로케이터 소유 생성
    _locationType = locatorShape;
    switch (_locationType)
    {
    case LocationShape::SPHERE:
        _emitLocator = std::make_unique<SphereLocator>();
        break;
    case LocationShape::CUBE:
        _emitLocator = std::make_unique<CubeLocator>();
        break;
    case LocationShape::CYLINDER:
        _emitLocator = std::make_unique<CylinderLocator>();
        break;
    case LocationShape::TORUS:
        _emitLocator = std::make_unique<TorusLocator>();
        break;
    case LocationShape::CONE:
        _emitLocator = std::make_unique<ConeLocator>();
        break;
    case LocationShape::MESH_SURFACE:
        _emitLocator = std::make_unique<MeshSurfaceLocator>();
        break;
    default:
        _emitLocator = std::make_unique<SphereLocator>();
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
    if (!_delayFlag)
    {
        _delayTimer += deltaTime;
        if (_delayTimer >= _startDelay)
        {
            _activeFlag = true;
            _delayFlag  = true;
        }
        else
            return;
    }

    _emitterAge += deltaTime;
    if (_emitterAge >= _emitterLifetime - _particleLifetime)
        _endFlag = true;
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

    float denom = _emissionRate * _particleLifetime;
    if (_useLight)
    {
        if (denom > 0.0f)
        {
			_lightCurrentIntensity = (float)std::lerp(0.0f, _lightIntensity, _activeParticleCount / denom);
        }
        _lightCurrentRange     = _lightRange;
    }
}

void ParticleEmitter::UpdateParticleLifeCycle(float deltaTime)
{
    // 수명 만료 파티클 제거 (풀 앞쪽 구간만 활성)
    for (UINT i = 0; i < _activeParticleCount; ++i)
    {
        _particlePool[i].SetAge(_particlePool[i].GetAge() + deltaTime);
        if (_particlePool[i].GetAge() >= _particleLifetime)
        {
            _activeParticleCount--;
            std::swap(_particlePool[i], _particlePool[_activeParticleCount]);
            --i;
        }
        else
        {
            if (auto spriteModule = _particleRenderModule->AsSprite())
            {
                if (spriteModule->GetFrameInfo().z > 0)
                {
                    const Vector4& atlasInfo     = spriteModule->GetFrameInfo();
                    float          particleAge   = _particlePool[i].GetAge();
                    float          frameDuration = spriteModule->GetFrameDuration();
                    UINT           frameIndex    = static_cast<UINT>(particleAge / frameDuration);
                    if (_dragForce.x > 0)
                    {
                        frameIndex = frameIndex % static_cast<UINT>(atlasInfo.z);
                    }
                    else if (frameIndex >= atlasInfo.z)
                    {
                        frameIndex = static_cast<UINT>(atlasInfo.z - 1);
                    }
                    _particlePool[i].SetFrameinfo(spriteModule->GetCurrentFrameInfo(frameIndex));
                }
                else
                {
                    _particlePool[i].SetFrameinfo({-1, 0, 0, 0});
                }
            }
        }
    }

    if (_endFlag)
    {
        if (_activeParticleCount == 0)
            _activeFlag = false;
        for (UINT i = 0; i < _activeParticleCount; ++i)
        {
            _particlePool[i].SetAge(_particleLifetime);
        }

        return;
    }

    // 새 파티클 생성
    size_t newParticles = 0;
    _emissionThreshold += deltaTime * _emissionRate;

    if (_spawnBurstFlag && !_isSpawnBursted)
    {
        _emissionThreshold += _spawnBurstCount;
        _isSpawnBursted = true;
    }
    if (_emissionThreshold >= 1.0f)
    {
        newParticles = static_cast<size_t>(_emissionThreshold);
        _emissionThreshold -= newParticles;
    }

    size_t availableSlots = _maxParticles - _activeParticleCount;
    if (newParticles > availableSlots)
        newParticles = availableSlots;

    for (size_t i = 0; i < newParticles; ++i)
    {
        AwakeParticle(static_cast<UINT>(_activeParticleCount));
        _activeParticleCount++;
    }
}

void ParticleEmitter::FlushTextureResource()
{
    if (_particleType == ParticleType::SPRITE)
    {
        auto* spritemodule = static_cast<SpriteModule*>(_particleRenderModule.get());
        if (spritemodule->GetTextureChangeFlag())
        {
            spritemodule->SetTextureChangeFlag(false);
            spritemodule->SetAlbedoTexture(
                Global::resourceManager->LoadResource<Texture>(spritemodule->GetModelAndTexturePath()));
            Global::particleManager->RefreshEditor();
        }
    }
    if (_particleType == ParticleType::RIBBON)
    {
        auto* ribbonmodule = static_cast<RibbonModule*>(_particleRenderModule.get());
        if (ribbonmodule->GetTextureChangeFlag())
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
        _emitLocator = std::make_unique<SphereLocator>();
        break;
    case LocationShape::CUBE:
        _emitLocator = std::make_unique<CubeLocator>();
        break;
    case LocationShape::CYLINDER:
        _emitLocator = std::make_unique<CylinderLocator>();
        break;
    case LocationShape::CONE:
        _emitLocator = std::make_unique<ConeLocator>();
        break;
    case LocationShape::TORUS:
        _emitLocator = std::make_unique<TorusLocator>();
        break;
    case LocationShape::MESH_SURFACE:
        _emitLocator = std::make_unique<MeshSurfaceLocator>();
        break;
    }

    _emitLocator->SetFactor(factor);
    _emitLocator->RandomInitialize();
}

void ParticleEmitter::AwakeParticle(UINT index)
{
    // 로케이터 존재 가드
    if (!_emitLocator)
        return;

    Vector3 offset   = {_emitLocator->RandomVal(), _emitLocator->RandomVal(), _emitLocator->RandomVal()};
    Vector4 location = {1, 1, 1, 1};
    Vector3 tempPos  = _emitLocator->EmitLocate();

    float ratio = 0.f;
    if (_emitterLifetime <= _particleLifetime)
        ratio = (_emitterLifetime > 0.f) ? (_emitterAge / _emitterLifetime) : 0.f;
    else
        ratio = (_emitterAge / (_emitterLifetime - _particleLifetime));

    Vector3 currentOffset = Vector3::Lerp(_particleStartDistributionOffset, _particleEndDistributionOffset, ratio);

    location.x = tempPos.x + offset.x * std::max(0.f, currentOffset.x);
    location.y = tempPos.y + offset.y * std::max(0.f, currentOffset.y);
    location.z = tempPos.z + offset.z * std::max(0.f, currentOffset.z);

    if (_useWorldSpace)
        location = Vector4::Transform(location, _worldMatrix);

    _particlePool[index].SetPosition(location);
    ScaleVelocity({location.x, location.y, location.z});

    Vector3 finalVelocity = Vector3::TransformNormal(_velocity, _worldMatrix);
    _particlePool[index].SetVelocity(finalVelocity);
    _particlePool[index].SetAxis(GetScaleByVelocityFlag() ? finalVelocity : _particleAxis);
    _particlePool[index].SetAge(0.f);
    _particlePool[index].SetMass(_particleMass);

    if (_particleType == ParticleType::SPRITE)
    {
        if (auto spriteModule = _particleRenderModule->AsSprite())
        {
            if (spriteModule->GetFrameInfo().z > 0)
            {
                const Vector4& atlasInfo   = spriteModule->GetFrameInfo();
                float          particleAge = _particlePool[index].GetAge();
                float          frameDuration = spriteModule->GetFrameDuration();
                UINT           frameIndex  = static_cast<UINT>(particleAge /frameDuration);
                if (_dragForce.x > 0)
                {
                    frameIndex = frameIndex % static_cast<UINT>(atlasInfo.z);
                }
                else if (frameIndex >= atlasInfo.z)
                {
                    frameIndex = static_cast<UINT>(atlasInfo.z - 1);
                }
                _particlePool[index].SetFrameinfo(spriteModule->GetCurrentFrameInfo(frameIndex));
            }
            else
            {
                _particlePool[index].SetFrameinfo({-1, 0, 0, 0});
            }
        }
    }
    _particlePool[index].SetInitialMatrix(_worldMatrix.Transpose());
    _particlePool[index].SetSpriteRotation(_particleRotation);
}

void ParticleEmitter::ScaleVelocity(Vector3 pos)
{
    switch (_velocityType)
    {
    case VelocityScaleType::LINEAR:
        _velocity = _velocityFactor;
        break;
    case VelocityScaleType::CONE:
        [[fallthrough]];
    case VelocityScaleType::POINT:
        ScaleVelFromPoint(pos);
        break;
    case VelocityScaleType::CUSTOM:
        _velocity = _velocityScalingFunciton ? _velocityScalingFunciton() : _velocityFactor;
        break;
    default:
        _velocity = _velocityFactor;
        break;
    }
}

void ParticleEmitter::ScaleVelFromPoint(Vector3 pos)
{
    Vector3 direction = pos;
    if (direction.LengthSquared() > 0.f)
        direction.Normalize();
    _velocity = direction * _velocityFactor.x;
}

void ParticleEmitter::ScaleVelInCone(Vector3 /*pos*/)
{
    // TODO: 필요 시 구현
}

void ParticleEmitter::InitializeLight(std::string_view scenenName)
{
    if (true == _useLight)
    {
        _particlePointLight = new Light();
        _particlePointLight->SetActive(&_activeFlag);
        _particlePointLight->SetPointLight(_lightColor, _finalPos, _lightAttenuation, _lightCurrentRange,
                                           _lightCurrentIntensity);
        Global::lightCore->RegisterLight(scenenName, static_cast<Light*>(_particlePointLight.Get()));
        if ("Game" == scenenName)
        {
            Global::lightCore->RegisterLight("Editor", static_cast<Light*>(_particlePointLight.Get()));
        }
    }

}

void ParticleEmitter::InitializeEditorLight()
{
    _particlePointLight = new Light();
    _particlePointLight->SetActive(&_activeFlag);
    _particlePointLight->SetPointLight(_lightColor, _finalPos, _lightAttenuation, _lightCurrentRange,
                                       _lightCurrentIntensity);
    Global::lightCore->RegisterLight("ParticleEditor", static_cast<Light*>(_particlePointLight.Get()));
}
