#include "pchScripts.h"
#include "TextureLMH.h"
#include "PreferencesButton.h"
#include "Scripts/Preferences/PreferencesManager.h"

UMREAL_COMPONENT(TextureLMH)

TextureLMH::TextureLMH() {}
TextureLMH::~TextureLMH() = default;

void TextureLMH::Awake()
{
    // 자주사용할 게임오브젝트 포인터
    GetChildObject();

    // 양옆 화살표 숨기기
    if (_leftArrow)
        _leftArrow->SetActive(false);

    if (_rightArrow)
        _rightArrow->SetActive(false);

    // 포커스 / 논포커스 아이콘 비활성화
    for (size_t i = 0; i < TextureQuality::TEXTURE_QUALITY_END; ++i)
    {
        if (_focus[i])
            _focus[i]->SetActive(false);

        if (_nonFocus[i])
            _nonFocus[i]->SetActive(false);
    }

    _quality = UmPreferences.GetTextureQuality();
    SetQuality(_quality);
}


void TextureLMH::Start()
{
    // 관리 매니저 객체
    GameObject* manager = GameObject::Find("PreferencesManager").lock().get();
    if (manager)
    {
        _preferencesManager = manager->GetComponent<PreferencesManager>();
        if (nullptr == _preferencesManager)
            UmLogger.Log(LogLevel::LEVEL_WARNING, "Preferences manager not registered!");
    }
    else
    {
        UmLogger.Log(LogLevel::LEVEL_WARNING, "Preferences manager not registered!");
    }
}

void TextureLMH::Reset()
{
    UINavigationComponent::Reset();

    BindInputAction(ControllerButton::DPAD_RIGHT, Action::PRESSED, this, &TextureLMH::UpQuality);
    BindInputAction(ControllerButton::DPAD_LEFT, Action::PRESSED, this, &TextureLMH::DownQuality);
    BindInputAction(ControllerButton::LEFT_THUMB_STICK, Action::PRESSED, this, &TextureLMH::UpDownStickQuality);
}

void TextureLMH::Update()
{
    if (_isOptionDirty)
    {
        if (_isFocus)
        {
            FocusPref(true);

            if (_leftArrow)
                _leftArrow->SetActive(true);

            if (_rightArrow)
                _rightArrow->SetActive(true);

            if ( _nonFocus[_quality])
                _nonFocus[_quality]->SetActive(false);

            if (_isOptionUp)
            {
                if (& _focus[_quality])
                    _focus[_quality]->SetActive(false);

                _quality    = (_quality + 1) % TextureQuality::TEXTURE_QUALITY_END;
                _isOptionUp = false;
            }
            else if (_isOptionDown)
            {
                if (_focus[_quality])
                    _focus[_quality]->SetActive(false);

                _quality      = (_quality + 2) % TextureQuality::TEXTURE_QUALITY_END;
                _isOptionDown = false;
            }

            if (_quality < TextureQuality::TEXTURE_QUALITY_END)
                SetQuality(_quality);
        }
        else
        {
            FocusPref(false);

            if (_leftArrow)
                _leftArrow->SetActive(false);

            if (_rightArrow)
                _rightArrow->SetActive(false);

            // 포커스 나갈 때 한번 설정
            if (_quality < TextureQuality::TEXTURE_QUALITY_END)
                SetQuality(_quality);
        }
    }
}

void TextureLMH::FocusIn(const FocusCallType callType)
{
    Base::FocusIn(callType);

    _isFocus       = true;
    _isOptionDirty = true;
}

void TextureLMH::FocusOut(const FocusCallType callType)
{
    Base::FocusOut(callType);

    _isFocus       = false;
    _isOptionDirty = true;
}

void TextureLMH::Submit()
{
    Base::Submit();
}

void TextureLMH::SerializedReflectEvent() {}

void TextureLMH::DeserializedReflectEvent()
{
    UINavigationComponent::DeserializedReflectEvent();
}

void TextureLMH::FocusPref(const bool isfocus)
{
    if (nullptr == _pref)
    {
        GetChildObject();
    }
    auto prefButton = _pref->GetComponent<PreferencesButton>();
    if (prefButton)
        prefButton->OnFocus(isfocus);
}

void TextureLMH::GetChildObject()
{
    // 자주사용할 게임오브젝트 포인터
    int childCnt = transform->Parent->GetChildCount();
    for (int i = 0; i < childCnt; ++i)
    {
        Transform* child = transform->Parent->GetChild(i);
        if (nullptr == child)
            continue;
        if (child->gameObject->CompareTag("LeftArrow"))
        {
            _leftArrow = &(child->gameObject);
        }
        if (child->gameObject->CompareTag("RightArrow"))
        {
            _rightArrow = &(child->gameObject);
        }
        if (child->gameObject->CompareTag("Pref"))
        {
            _pref = &(child->gameObject);
        }
    }

    childCnt = transform->GetChildCount();
    for (int i = 0; i < childCnt; ++i)
    {
        Transform* child = transform->GetChild(i);
        if (nullptr == child)
            continue;
        if (child->gameObject->CompareTag("LowFocus"))
        {
            _focus[TextureQuality::TEXTURE_QUALITY_LOW] = &(child->gameObject);
        }
        if (child->gameObject->CompareTag("LowNonFocus"))
        {
            _nonFocus[TextureQuality::TEXTURE_QUALITY_LOW] = &(child->gameObject);
        }
        if (child->gameObject->CompareTag("MediumFocus"))
        {
            _focus[TextureQuality::TEXTURE_QUALITY_MEDIUM] = &(child->gameObject);
        }
        if (child->gameObject->CompareTag("MediumNonFocus"))
        {
            _nonFocus[TextureQuality::TEXTURE_QUALITY_MEDIUM] = &(child->gameObject);
        }
        if (child->gameObject->CompareTag("HighFocus"))
        {
            _focus[TextureQuality::TEXTURE_QUALITY_HIGH] = &(child->gameObject);
        }
        if (child->gameObject->CompareTag("HighNonFocus"))
        {
            _nonFocus[TextureQuality::TEXTURE_QUALITY_HIGH] = &(child->gameObject);
        }
    }
}

void TextureLMH::SetQuality(const int quality)
{
    // quality 범위 체크
    if (quality < 0 || quality >= TextureQuality::TEXTURE_QUALITY_END)
        return;

    if (_isFocus)
    {
        switch (quality)
        {
        case TEXTURE_QUALITY_LOW:
            if (_focus[TEXTURE_QUALITY_LOW])
                _focus[TEXTURE_QUALITY_LOW]->SetActive(true);

            if (_preferencesManager)
                _preferencesManager->SetGraphicsQuality(PreferencesSystem::TextureQuality::LOW);
            break;

        case TEXTURE_QUALITY_MEDIUM:
            if (_focus[TEXTURE_QUALITY_MEDIUM])
                _focus[TEXTURE_QUALITY_MEDIUM]->SetActive(true);

            if (_preferencesManager)
                _preferencesManager->SetGraphicsQuality(PreferencesSystem::TextureQuality::MEDIUM);
            break;

        case TEXTURE_QUALITY_HIGH:
            if (_focus[TEXTURE_QUALITY_HIGH])
                _focus[TEXTURE_QUALITY_HIGH]->SetActive(true);

            if (_preferencesManager)
                _preferencesManager->SetGraphicsQuality(PreferencesSystem::TextureQuality::HIGH);
            break;

        default:
            break;
        }
    }
    else // !_isFocus
    {
        switch (quality)
        {
        case TEXTURE_QUALITY_LOW:
            if (_nonFocus[TEXTURE_QUALITY_LOW])
                _nonFocus[TEXTURE_QUALITY_LOW]->SetActive(true);
            break;

        case TEXTURE_QUALITY_MEDIUM:
            if (_nonFocus[TEXTURE_QUALITY_MEDIUM])
                _nonFocus[TEXTURE_QUALITY_MEDIUM]->SetActive(true);
            break;

        case TEXTURE_QUALITY_HIGH:
            if (_nonFocus[TEXTURE_QUALITY_HIGH])
                _nonFocus[TEXTURE_QUALITY_HIGH]->SetActive(true);
            break;

        default:
            break;
        }
    }
}


void TextureLMH::UpQuality(const Input::Controller& controller)
{
    if (_isFocus)
    {
        _isOptionDirty = true;
        _isOptionUp    = true;
        UmAudio.Play("-40000");
    }
}

void TextureLMH::DownQuality(const Input::Controller& controller)
{
    if (_isFocus)
    {
        _isOptionDirty = true;
        _isOptionDown  = true;
        UmAudio.Play("-40000");
    }
}

void TextureLMH::UpDownStickQuality(const Input::Controller& controller)
{
    if (Input::Controller::StickBias::BIAS_LEFT == controller.GetLeftStickBias())
    {
        DownQuality(controller);
    }
    else if (Input::Controller::StickBias::BIAS_RIGHT == controller.GetLeftStickBias())
    {
        UpQuality(controller);
    }
}
