#include "pchScripts.h"
#include "TokenHUDView.h"
#include "UI/Elements/Text/TextElement.h"

UMREAL_COMPONENT(TokenHUDTextView)

TokenHUDTextView::TokenHUDTextView() = default;

TokenHUDTextView::~TokenHUDTextView() = default;

void TokenHUDTextView::Watch(const std::string& key)
{
    if (false == key.empty())
    {
        UmWatcher.Blind<TokenHUDViewModel>(key, _handle);

        try
        {
            if (TextElement* textElement = GetComponent<TextElement>())
            {
                _textElement = textElement;
            }

            _handle = UmWatcher.Watch<TokenHUDViewModel, int>(key, [this](const int& value) {
                if (_textElement)
                {
                    bool isEnable        = value > 0;
                    _textElement->Enable = isEnable;
                    _textElement->Text   = std::to_string(value);
                }
            });

            _key = key;
        }
        catch (const std::exception& e)
        {
            UmLogger.Log(LogLevel::LEVEL_ERROR, "Watch Failed.");
            UmLogger.Log(LogLevel::LEVEL_ERROR, e.what());
            _key.clear();
        }
    }
    else
    {
        UmLogger.Log(LogLevel::LEVEL_ERROR, "TokenHUDTextView: WatchKey is empty.");
        _key.clear();
    }
}

void TokenHUDTextView::OnDestroy()
{
    if (false == _key.empty())
    {
        UmWatcher.Blind<TokenHUDViewModel>(_key, _handle);
    }
}