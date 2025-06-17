#include "TestState.h"

REGISTER_CLASS(FSMStateFactory, TestMoveState)

TestMoveState::TestMoveState() 
{

}

TestMoveState::~TestMoveState() 
{

}

void TestMoveState::OnAwake() 
{

}

void TestMoveState::OnStart() 
{

}

void TestMoveState::OnEnter() 
{
    UmLogger.Log(LogLevel::LEVEL_DEBUG, "Move Enter");
}

void TestMoveState::OnExit() 
{
    UmLogger.Log(LogLevel::LEVEL_DEBUG, "Move Exit");
}

void TestMoveState::OnUpdate() 
{
    UmLogger.Log(LogLevel::LEVEL_DEBUG, "Move Update");
}
