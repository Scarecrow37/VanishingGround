#include "pch.h"

#ifdef _UMEDITOR
namespace Global
{
    constexpr bool IsPlay()
    {
        if constexpr (Application::IsEditor())
        {
            return ESceneManager::Engine::IsPlayMode(UmSceneManager);
        }
        else
        {
            return true;
        }
    }
} // namespace Global
#endif