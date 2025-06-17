#include "TestCondition.h"

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

}

bool TestWorkCondition::Evaluate()
{
    return false;
}
