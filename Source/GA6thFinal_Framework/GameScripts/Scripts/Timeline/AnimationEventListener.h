#pragma once
#include "UmFramework.h"

class SkeletalMeshRenderer;

class AnimationEventListener : public Component
{
    USING_PROPERTY(AnimationEventListener)

public:
    AnimationEventListener();
    virtual ~AnimationEventListener();

public:
    REFLECT_PROPERTY(FilePath, FileGuid)
    GETTER_ONLY(std::string, FilePath) { return _filePath.string(); }
    PROPERTY(FilePath)
    GETTER_ONLY(std::string, FileGuid) { return _guidRef.string(); }
    PROPERTY(FileGuid)

public:
    void Start() override;

private:
    void SetNotifySetGuid(File::Guid guid);
    void SetNotifySetPath(File::Path path);

protected:
    File::GuidRef _guidRef;
    File::Path    _filePath;

    SkeletalMeshRenderer* _skeletalMeshRenderer = nullptr;

    REFLECT_FIELDS_BEGIN(Component)
    std::string Guid;
    REFLECT_FIELDS_END(AnimationEventListener)
};
