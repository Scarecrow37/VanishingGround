#pragma once
#include "IGraphicsBase.h"

class IAnimator : public IGraphicsBase
{
protected:
    IAnimator()          = default;
    virtual ~IAnimator() = default;

public:
    virtual const Matrix*                   FindBoneMatrix(const char* boneName) const             = 0;
    virtual bool                            IsPaused() const                                       = 0;
    virtual bool                            IsLoop() const                                         = 0;
    virtual bool                            IsEnd() const                                          = 0;
    virtual bool                            IsLastFrame(float interval, unsigned int ID) const     = 0;
    virtual bool                            HasAnimation(const char* animation) const              = 0;
    virtual const std::vector<const char*>& GetAnimationNames() const                              = 0;
    virtual float                           GetAnimationLastTime(const char* animation) const      = 0;
    virtual float                           GetCurrentAnimationLastTime(unsigned int ID = 0) const = 0;
    virtual float                           GetCurrentAnimationPlayTime(unsigned int ID = 0) const = 0;
    virtual float                           GetCurrentAnimationSpeed(unsigned int ID = 0) const    = 0;

public:
    virtual void SetAnimationTime(float time)                            = 0;
    virtual void SetAnimationTime(float time, unsigned int ID)           = 0;
    virtual void SetAnimationSpeed(float speed)                          = 0;
    virtual void SetAnimationSpeed(float speed, unsigned int ID)         = 0;
    virtual void SetPause(bool isPause)                                  = 0;
    virtual void SetLoop(bool isLoop)                                    = 0;
    virtual void SetAnimationEndCallback(std::function<void()> callback) = 0;

public:
    virtual void Update(float deltaTime)                                                       = 0;
    virtual bool ChangeAnimation(const char* animation, bool blending = true)                  = 0;
    virtual bool ChangeAnimation(const char* animation, unsigned int ID, bool blending = true) = 0;
    virtual void SyncPartialAnimation(unsigned int parentID, unsigned int childID)             = 0;
    virtual void SetUpSplitBone(unsigned int maxSplit)                                         = 0;
    virtual void SplitBone(unsigned int ID, const char* boneName)                              = 0;
    virtual void MakeParent(const char* parent, const char* child)                             = 0;
};