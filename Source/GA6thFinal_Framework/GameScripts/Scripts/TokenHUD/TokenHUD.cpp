#include "pchScripts.h"
#include "TokenHUD.h"
#include "UI/Elements/Image/ImageElement.h"
#include "ViewModels/TokenHUD/TokenHUDViewModel.h"
#include "UI/Views/TokenHUD/TokenHUDView.h"

UMREAL_COMPONENT(TokenHUD)

TokenHUD::TokenHUD() = default;

TokenHUD::~TokenHUD() = default;

void TokenHUD::SetupTokenHUD(const File::Guid& guid, MVVM::Model<int>& tokenStack, const std::string& key)
{
    if (guid.IsNull() || key.empty())
    {
        UmLogger.Log(LogLevel::LEVEL_ERROR, "TokenHUD: SetupTokenHUD failed due to invalid guid or key.");
        return;
    }

    UmWatcher.Unregister<TokenHUDViewModel>(key);
    UmWatcher.Register<TokenHUDViewModel>(key, tokenStack);

    Transform::ForeachBFS(transform, [&](Transform* tr) {
        GameObject& object = tr->gameObject;
        if (object.CompareTag("Token Image"))
        {
            if (ImageElement* imageElement = object.GetComponent<ImageElement>())
            {
                imageElement->SetImage(guid);
            }
        }

        if (object.CompareTag("Token Text"))
        {
            if (TokenHUDTextView* tokenHUDTextView = object.GetComponent<TokenHUDTextView>())
            {
                tokenHUDTextView->Watch(key);
            }
        }
    });

    _key = key;
}

void TokenHUD::OnDestroy()
{
    if (!_key.empty())
    {
        UmWatcher.Unregister<TokenHUDViewModel>(_key);
    }
}