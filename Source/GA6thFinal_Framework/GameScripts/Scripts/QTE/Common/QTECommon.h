#pragma once
#include "QTE/Result/QTEResult.h"

namespace QTE
{
    struct OverallResult;
    class Callback
    {
    public:
        using Handle = int;
        Callback(std::weak_ptr<Component> weakOwner) 
            : Owner(weakOwner) {}
        Callback()  = default;
        ~Callback() = default;

        inline bool Invalid() { return Owner.expired(); }

        std::weak_ptr<Component> Owner;

        std::function<void()>                           OnFadeInStart   = nullptr;
        std::function<void()>                           OnFadeInFinish  = nullptr;
        std::function<void(const QTE::OverallResult&)>  OnFadeOutStart  = nullptr;
        std::function<void(const QTE::OverallResult&)>  OnFadeOutFinish = nullptr;
        std::function<void()>                           OnButtonPressed = nullptr;
        std::function<void(UINT, QTE::ResultType)>      OnNotePressed   = nullptr;
    };

    enum PlayState
    {
        STATE_WAITING,
        STATE_FADE_IN,
        STATE_PLAYING,
        STATE_FADE_OUT,
    };
}
