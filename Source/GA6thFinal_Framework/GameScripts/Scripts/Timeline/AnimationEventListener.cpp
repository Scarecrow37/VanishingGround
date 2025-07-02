#include "pchScripts.h"
#include "AnimationEventListener.h"
#include "Scripts/Mesh/SkeletalMeshRenderer.h"

AnimationEventListener::AnimationEventListener() = default;
AnimationEventListener::~AnimationEventListener() = default;

void AnimationEventListener::Start()
{
    _skeletalMeshRenderer = GetComponent<SkeletalMeshRenderer>();
}

void AnimationEventListener::SetNotifySetGuid(File::Guid guid)
{
    _guidRef            = guid;
    _filePath           = _guidRef.ToPath();
    ReflectFields->Guid = _guidRef.string();
}

void AnimationEventListener::SetNotifySetPath(File::Path path) 
{
    _filePath           = path;
    _guidRef            = path.ToGuid();
    ReflectFields->Guid = _guidRef.string();
}
