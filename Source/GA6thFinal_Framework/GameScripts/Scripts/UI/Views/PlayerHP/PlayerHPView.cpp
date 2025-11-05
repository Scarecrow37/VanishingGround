#include "pchScripts.h"
#include "PlayerHPView.h"
#include "UI/Elements/Image/ImageElement.h"
#include "UI/Elements/Text/TextElement.h"
#include "Stats/Player/PlayerStats.h"
#include "PlayerSystem/PlayerSystem.h"
#include "UI/Animations/ReduceGage/ReduceGage.h"

UMREAL_COMPONENT(PlayerHPTextView)
UMREAL_COMPONENT(PlayerHPImageView)

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
        Transform::ForeachBFS(transform, [this](Transform* tr) {
            if (GameObject& object = tr->gameObject; object.CompareTag("HP Bar"))
            {
                if (ImageElement* element = object.GetComponent<ImageElement>(); nullptr != element)
                {
                    _hpImageElement = element;
                }
            }
            if (GameObject& object = tr->gameObject; object.CompareTag("Reduce HP Bar"))
            {
                if (ImageElement* element = object.GetComponent<ImageElement>(); nullptr != element)
                {
                    _reduceHpImageElement = element;
                }

                _reduceGage = object.GetComponent<ReduceGage>();
            }
        });        

        _handle = UmWatcher.Watch<CharacterHPViewModel, CharacterHP>( PlayerStats::MODEL_HP_KEY, [this](const CharacterHP value) 
        { 
            float currentRate = (float)value.CurrentHP / (float)value.MaxHP;

            if (_hpImageElement)
            {
                _hpImageElement->SetLinearFill(currentRate);
            }

            if (_reduceGage)
            {
                _reduceGage->StartReduceGage(_reduceHpImageElement, currentRate);
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