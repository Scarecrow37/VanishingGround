#include "TestMoveCondition.h"
#include "Test/Component/TestComponent.h"

REGISTER_CLASS(FSMConditionFactory, TestMoveCodition)

TestMoveCodition::TestMoveCodition() {}

TestMoveCodition::~TestMoveCodition() {}

void TestMoveCodition::OnAwake() {}

void TestMoveCodition::OnStart() 
{
    testComponent = GetFSM().GetComponent<TestComponent>();
}

bool TestMoveCodition::Evaluate()
{
    if (testComponent)
    {
        return testComponent->Move >= 10;
    }
    return false;
}
