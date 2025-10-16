#include "pchScripts.h"
#include "PlayerHPView.h"
#include "UI/Elements/Image/ImageElement.h"
#include "UI/Elements/Text/TextElement.h"
#include "Stats/Player/PlayerStats.h"
#include "PlayerSystem/PlayerSystem.h"

UMREAL_COMPONENT(PlayerHPTextView)
UMREAL_COMPONENT(PlayerHPImageView)

PlayerHPTextView::PlayerHPTextView()  = default;

PlayerHPTextView::~PlayerHPTextView()
{
   
}

void PlayerHPTextView::Start()
{    
    try
    {
        _hp = GetComponent<TextElement>();
        _handle = UmWatcher.Watch<CharacterHPViewModel, CharacterHP>(PlayerStats::MODEL_HP_KEY, [this](const CharacterHP value) 
        {
            if (_hp)
            {
                _hp->Text = std::format("{} / {}", value.CurrentHP, value.MaxHP);
            }
        });
    }
    catch (const std::exception& e)
    {
        UmLogger.Log(LogLevel::LEVEL_ERROR, "Watch Failed.");
        UmLogger.Log(LogLevel::LEVEL_ERROR, e.what());
    }
    if (PlayerSystem* system = SingletonComponent<PlayerSystem>::GetInstance())
    {
        system->NotifyPlayerHP();
    }
}

void PlayerHPTextView::OnDestroy() 
{
    UmWatcher.Blind<CharacterHPViewModel>(PlayerStats::MODEL_HP_KEY, _handle);
}


PlayerHPImageView::PlayerHPImageView() 
{

}

PlayerHPImageView::~PlayerHPImageView() 
{

}

void PlayerHPImageView::Start() 
{
    try
    {
        _hpGage = GetComponent<ImageElement>();
        _handle = UmWatcher.Watch<CharacterHPViewModel, CharacterHP>(PlayerStats::MODEL_HP_KEY, [this](const CharacterHP value) 
        { 
            if (_hpGage)
            {
                _hpGage->SetLinearFill(value.CurrentHP / (float)value.MaxHP);
            }
        });
        if (PlayerSystem* system = SingletonComponent<PlayerSystem>::GetInstance())
        {
            system->NotifyPlayerHP();
        }
    }
    catch (const std::exception& e)
    {
        UmLogger.Log(LogLevel::LEVEL_ERROR, "Watch Failed.");
        UmLogger.Log(LogLevel::LEVEL_ERROR, e.what());
    }
}

void PlayerHPImageView::OnDestroy() 
{
    UmWatcher.Blind<CharacterHPViewModel>(PlayerStats::MODEL_HP_KEY, _handle);
}
