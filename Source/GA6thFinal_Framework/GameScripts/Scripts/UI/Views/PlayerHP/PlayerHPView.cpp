#include "pchScripts.h"
#include "PlayerHPView.h"
#include "UI/Elements/Image/ImageElement.h"
#include "UI/Elements/Text/TextElement.h"
#include "Stats/Player/PlayerStats.h"
#include "PlayerSystem/PlayerSystem.h"
#include "UI/Animations/ReduceGage/ReduceGage.h"

UMREAL_COMPONENT(PlayerHPTextView)
UMREAL_COMPONENT(PlayerHPImageView)
UMREAL_COMPONENT(PlayerHpReduceImageView)

PlayerHPTextView::PlayerHPTextView()  = default;

PlayerHPTextView::~PlayerHPTextView() = default;

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


PlayerHPImageView::PlayerHPImageView() = default;

PlayerHPImageView::~PlayerHPImageView() = default;

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

PlayerHpReduceImageView::PlayerHpReduceImageView() = default;

 PlayerHpReduceImageView::~PlayerHpReduceImageView() = default;

void PlayerHpReduceImageView::Start()
{
    try
    {
        if (_reduceHpImageElement)
            _reduceHpImageElement->Enable = true;

        _reduceHpImageElement = GetComponent<ImageElement>();
        _reduceGage           = GetComponent<ReduceGage>();
        _handle               = UmWatcher.Watch<CharacterHPViewModel, CharacterHP>(
            PlayerStats::MODEL_REDUCE_HP_KEY, [this](const CharacterHP& value) {
                float currentRate = (float)value.CurrentHP / (float)value.MaxHP;

                if (_reduceGage)
                {
                    _reduceGage->StartReduceGage(_reduceHpImageElement, currentRate);
                }
            });
    }
    catch (const std::exception& e)
    {
        UmLogger.Log(LogLevel::LEVEL_ERROR, "Watch Failed.");
        UmLogger.Log(LogLevel::LEVEL_ERROR, e.what());
    }
}

void PlayerHpReduceImageView::OnDestroy()
{
    UmWatcher.Blind<CharacterHPViewModel>(PlayerStats::MODEL_REDUCE_HP_KEY, _handle);
}