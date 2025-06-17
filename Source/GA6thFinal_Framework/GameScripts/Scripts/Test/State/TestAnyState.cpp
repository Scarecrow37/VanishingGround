#include "TestAnyState.h"

REGISTER_CLASS(FSMStateFactory, TestAnyState)

TestAnyState::TestAnyState() {}

TestAnyState::~TestAnyState() {}

void TestAnyState::OnAwake() {}

void TestAnyState::OnStart() {}

void TestAnyState::OnEnter() 
{
    UmLogger.Log(LogLevel::LEVEL_DEBUG, "Any Enter");
}

void TestAnyState::OnExit() 
{
    UmLogger.Log(LogLevel::LEVEL_DEBUG, "Any Exit");
}

void TestAnyState::OnUpdate() 
{

}
