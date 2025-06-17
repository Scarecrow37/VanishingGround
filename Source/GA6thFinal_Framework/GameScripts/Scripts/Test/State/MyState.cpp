#include "MyState.h"

REGISTER_CLASS(FSMStateFactory, TesWorkState)

TesWorkState::TesWorkState() {}
TesWorkState::~TesWorkState() {}

void TesWorkState::OnAwake() {}

void TesWorkState::OnStart() {}


void TesWorkState::OnEnter() 
{
    UmLogger.Log(LogLevel::LEVEL_DEBUG, "Work Enter");
}

void TesWorkState::OnExit() 
{
    UmLogger.Log(LogLevel::LEVEL_DEBUG, "Work Exit");
}

void TesWorkState::OnUpdate() 
{
    UmLogger.Log(LogLevel::LEVEL_DEBUG, "Work Update");
}
