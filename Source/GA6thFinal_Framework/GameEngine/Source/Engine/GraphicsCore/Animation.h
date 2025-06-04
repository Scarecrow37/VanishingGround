#pragma once
#include "Resource.h"

class Animation : public Resource
{
    friend class Animator;

    struct BoneTransformTrack
    {
        std::vector<std::pair<float, Vector3>> Positions;
        std::vector<std::pair<float, Vector4>> Rotations;
        std::vector<std::pair<float, Vector3>> Scales;
    };

    struct Channel
    {
        std::unordered_map<std::string, BoneTransformTrack> BoneTransforms;
        float                                               LastTime = 0.f;
    };

public:
    explicit Animation() = default;
    virtual ~Animation() = default;

public:
    const std::vector<const char*>& GetAnimations() { return _animationNames; }

public:
    void LoadAnimation(const aiScene* scene);
    // Resource을(를) 통해 상속됨
    HRESULT LoadResource(const std::filesystem::path& filePath) override;

private:
    std::unordered_map<std::string, Channel> _animations;
    std::vector<const char*>                 _animationNames;
};