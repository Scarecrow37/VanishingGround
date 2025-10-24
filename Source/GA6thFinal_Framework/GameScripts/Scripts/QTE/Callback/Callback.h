#pragma once
#include "QTE/Common/QTECommon.h"
#include "QTE/Result/QTEResult.h"

namespace QTE
{
    struct OverallResult;

    class CallbackHandler
    {
    public:
        CallbackHandler() = default;
        ~CallbackHandler() = default;

        Callback::Handle RegisterCallback(const Callback& callback);
        bool             UnRegisterCallback(Callback::Handle handle);

        void ProcessQTEFadeInStartEvent();
        void ProcessQTEFadeInFinishEvent();
        void ProcessQTEFadeOutStartEvent(const QTE::OverallResult& result);
        void ProcessQTEFadeOutFinishEvent(const QTE::OverallResult& result);
        void ProcessQTEButtonPressedEvent();
        void ProcessQTENotePressedEvent(UINT noteID, QTE::ResultType result);

    private:
        int _handleCounter = 0;
        std::unordered_map<Callback::Handle, Callback> _callbackTable;
    };
}
