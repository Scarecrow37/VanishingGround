#include "pch.h"

#ifdef _UMEDITOR
namespace Global
{
    constexpr bool IsPlay()
    {
        if constexpr (Application::IsEditor())
        {
            return Global::editorModule->PlayMode.IsPlay();
        }
        else
        {
            return true;
        }
    }
} // namespace Global
#endif