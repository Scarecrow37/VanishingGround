#pragma once
#include "ViewModels/TokenHUD/TokenHUDViewModel.h"

class TextElement;
class TokenHUDTextView : public Component
{
    USING_PROPERTY(TokenHUDTextView)

public:
    TokenHUDTextView();
    ~TokenHUDTextView() override;

public:
    void Watch(const std::string& key);

protected:
    void OnDestroy() override;

protected:
    REFLECT_FIELDS_BEGIN(Component)
    REFLECT_FIELDS_END(TokenHUDTextView)

private:
    std::weak_ptr<TextElement> _textElement;
    TokenHUDViewModel::Handle  _handle;
    std::string                _key;
};