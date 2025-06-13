#pragma once
#include "UmFramework.h"
class BehaviorTree : public Component
{
    USING_PROPERTY(BehaviorTree)
public:
    REFLECT_PROPERTY()

public:
    BehaviorTree();
    virtual ~BehaviorTree();

public:
    inline void Test(){}

protected:
    REFLECT_FIELDS_BEGIN(Component)
    std::filesystem::path _behaviorAssetPath;
    REFLECT_FIELDS_END(BehaviorTree)
};
