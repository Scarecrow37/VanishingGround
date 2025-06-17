#include "TestCondition.h"
#include "Test/Component/TestComponent.h"

REGISTER_CLASS(FSMConditionFactory, TestWorkCondition)

TestWorkCondition::TestWorkCondition() 
{

}

TestWorkCondition::~TestWorkCondition()
{

}

void TestWorkCondition::OnAwake() 
{
    
}

void TestWorkCondition::OnStart() 
{
    testComponent = GetFSM().GetComponent<TestComponent>();
}

bool TestWorkCondition::Evaluate()
{
    if (testComponent)
    {
        return testComponent->Work >= 10;
    }
    return false;
}
