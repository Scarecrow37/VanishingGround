#include "pch.h"
#include "Particle.h"
#include "ParticleEmitter.h"
#include "ParticleEffect.h"

ParticleEffect::ParticleEffect()  = default;
ParticleEffect::~ParticleEffect() = default; // unique_ptr가 정리함

ParticleEmitter* ParticleEffect::AddEmitter(SIZE_T maxParticles, float emissionRate, float emitterLifetime,
                                            LocationShape locatorShape, Vector3 locationFactor,
                                            ParticleType        particleType,
                                            const std::wstring& meshspritePath) // view -> wstring
{
    auto&            uptr    = _particleEmitters.emplace_back(std::make_unique<ParticleEmitter>());
    ParticleEmitter* emitter = uptr.get(); // 외부에는 비소유 포인터 전달

    //최대 개수 강제 제한
    emitter->Initialize(10000, emissionRate, emitterLifetime, locatorShape, locationFactor, particleType,
                        meshspritePath);

    std::string name = "Emitter " + std::to_string(_namingIndex) + "-" + std::to_string(_emitterNamingIndex++);
    emitter->SetEmitterName(name);

    return emitter; 
}

void ParticleEffect::Update(float deltaTime)
{
    _age += deltaTime;
    if (_age >= _lifetime)
    {
        _activeFlag = false;
        for (auto& uptr : _particleEmitters)
        {
            if (uptr)
                uptr->SetActiveFlag(false);
        }
        _playFlag = false;
        if (_isPlaying)
        {
            _isPlaying = false;
            _age       = 0;
        }
        return;
    }

    _translationMatrix = (_position != nullptr) ? Matrix::CreateTranslation(*_position) : Matrix::Identity;
    _rotationMatrix    = (_rotation != nullptr)
                             ? Matrix::CreateFromQuaternion(Quaternion::CreateFromYawPitchRoll(*_rotation))
                             : Matrix::Identity;
    _scaleMatrix       = (_scale != nullptr) ? Matrix::CreateScale(*_scale) : Matrix::Identity;

    if (_parentWorldMatrix != nullptr)
    {
        if (_followBoneFlag != nullptr && (*_followBoneFlag == false))
        {
            _worldMatrix = _scaleMatrix * _rotationMatrix * _translationMatrix;
            _worldMatrix *= *_parentWorldMatrix;
        }
        else if (_boneWorldMatrix != nullptr)
        {
            _worldMatrix =
                _scaleMatrix * _rotationMatrix * _translationMatrix * (*_boneWorldMatrix) * (*_parentWorldMatrix);
        }
    }

    for (auto& uptr : _particleEmitters)
    {
        uptr->SetEffectWorldMatrix(_worldMatrix);
        uptr->Update(deltaTime);
    }

    // 모든 emitter가 비활성화되면 종료 상태로 전환
    {
        for (auto& uptr : _particleEmitters)
        {
            if (uptr->GetActiveFlag())
                return;
        }
        _activeFlag = false;
        _playFlag   = false;
        _isEnding   = false;
        if (_isPlaying)
        {
            _isPlaying = false;
            _age       = 0;
        }
        if (_endCallback)
        {
            _endCallback();
        }
    }
}

class ParticleEmitter* ParticleEffect::GetEmitter(size_t emitterIndex)
{
    // 범위 체크(안전)
    if (emitterIndex >= _particleEmitters.size())
        return nullptr;
    return _particleEmitters[emitterIndex] .get();
}

const std::vector<std::unique_ptr<ParticleEmitter>>& ParticleEffect::GetEmitterList() const
{
    return _particleEmitters;
}

void ParticleEffect::RemoveEmitter(ParticleEmitter* target)
{
    if (!target)
        return;
    target->SetRemoveFlag(true);
}

void ParticleEffect::UpdateParticleLifeCycle(float deltaTime)
{
    for (auto& uptr : _particleEmitters)
    {
        if (uptr->GetActiveFlag())
            uptr->UpdateParticleLifeCycle(deltaTime);
    }
}

void ParticleEffect::Play()
{
    if (!_isPlaying)
    {
        _playFlag   = true;
        _isPlaying  = true;
        _activeFlag = true;

        _isEnding = false;
        _age      = 0;
        for (auto& uptr : _particleEmitters)
        {
            uptr->Reset();
            uptr->SetActiveFlag(true);
        }
    }
}

void ParticleEffect::Play(EffectCallback callback) 
{
    if (!_isPlaying)
    {
        _playFlag   = true;
        _isPlaying  = true;
        _activeFlag = true;

        _isEnding = false;
        _age      = 0;
        for (auto& uptr : _particleEmitters)
        {
            uptr->Reset();
            uptr->SetActiveFlag(true);
        }
        _endCallback = callback;
    }
}

void ParticleEffect::Stop()
{
    if (!_isEnding)
    {
        _isEnding  = true;
        _isPlaying = false;
        for (auto& uptr : _particleEmitters)
        {
            uptr->SetEndFlag(true);
        }
    }
}

void ParticleEffect::Reset()
{
    _age = 0;
    for (auto& uptr : _particleEmitters)
        uptr->Reset();
    _activeFlag = true;
}

void ParticleEffect::FlushEmitters()
{
    std::erase_if(_particleEmitters, [](std::unique_ptr<ParticleEmitter>& p) { return p->GetRemoveFlag(); });
}

void ParticleEffect::SetRemoveFlag(bool flag)
{
    _removeFlag = flag;
}

bool ParticleEffect::GetRemoveFlag() const
{
    return _removeFlag;
}

void ParticleEffect::SetPosition(Vector3* position)
{
    _position = position;
}
void ParticleEffect::SetRotation(Vector3* rotation)
{
    _rotation = rotation;
}
void ParticleEffect::SetScale(Vector3* scale)
{
    _scale = scale;
}
void ParticleEffect::SetBoneFollowFlag(bool* flag)
{
    _followBoneFlag = flag;
}
void ParticleEffect::SetBoneMatrix(const Matrix* matrix)
{
    _boneWorldMatrix = matrix;
}
void ParticleEffect::SetParentMatrix(const Matrix* matrix) 
{
    _parentWorldMatrix = matrix;
}
