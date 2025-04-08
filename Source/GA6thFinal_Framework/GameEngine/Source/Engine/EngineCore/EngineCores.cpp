#include "pch.h"

EngineCores::EngineCores() = default;
EngineCores::~EngineCores() = default;

namespace Global
{
    SafeEngineCoresPtr engineCore{};
}

void SafeEngineCoresPtr::Engine::CreateEngineCores()
{
    if (Global::engineCore)
    {
        assert(!"엔진 코어가 이미 존재합니다.");
        return;
    }
    Global::engineCore = std::make_shared<EngineCores>();
}

void SafeEngineCoresPtr::Engine::DestroyEngineCores()
{
    Global::engineCore._instance.reset();
}

SafeEngineCoresPtr::SafeEngineCoresPtr() = default;
SafeEngineCoresPtr::~SafeEngineCoresPtr() = default;
