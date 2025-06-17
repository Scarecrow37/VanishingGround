#include "MyState.h"

REGISTER_CLASS(FSMStateFactory, TestWorkState)

TestWorkState::TestWorkState() {}
TestWorkState::~TestWorkState() {}

void TestWorkState::OnAwake() {}

void TestWorkState::OnStart() {}


void TestWorkState::OnEnter() 
{
    UmLogger.Log(LogLevel::LEVEL_DEBUG, "Work Enter");
}

void TestWorkState::OnExit() 
{
    UmLogger.Log(LogLevel::LEVEL_DEBUG, "Work Exit");
}

void TestWorkState::OnUpdate() 
{
    //UmLogger.Log(LogLevel::LEVEL_DEBUG, "Work Update");
}
