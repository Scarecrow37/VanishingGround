#include "TestMoveCondition.h"
#include "UmScripts.h"

TestMoveCodition::TestMoveCodition() {}

TestMoveCodition::~TestMoveCodition() {}

void TestMoveCodition::OnAwake() {}

void TestMoveCodition::OnStart() 
{
    testComponent = GetComponent<TestComponent>();
}

bool TestMoveCodition::Evaluate()
{
    return false;
}
