#include "ReflectHelper.h"
#include "pch.h"

using namespace ReflectHelper::ImGuiDraw;

void ReflectHelper::ImGuiDraw::Private::EngineLog(int logLevel, std::string_view message, std::source_location location)
{
    Global::engineCore->Logger.Log(logLevel, message, location);
}
