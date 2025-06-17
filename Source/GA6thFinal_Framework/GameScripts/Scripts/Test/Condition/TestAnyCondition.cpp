#include "TestAnyCondition.h"
#include "Test/Component/TestComponent.h"

REGISTER_CLASS(FSMConditionFactory, TestAnyCondition)

TestAnyCondition::TestAnyCondition() {}

TestAnyCondition::~TestAnyCondition() {}

void TestAnyCondition::OnAwake() {}

void TestAnyCondition::OnStart() 
{
    
}

bool TestAnyCondition::Evaluate()
{
    return 10 <= GetFSM().transform->Position->x;
}
