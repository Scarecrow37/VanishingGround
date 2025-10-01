#include "pchScripts.h"
#include "SoundButton.h"
#include "Scripts/Preferences/PreferencesManager.h"

UMREAL_COMPONENT(SoundButton)

SoundButton::SoundButton()
{
    CurrentOption.SetInputAutoEvent([this]() {
        if (ImGui::Combo("VolumeOptions", &_currentOptionInt, VolumeOptions.data(), (int)VolumeOptions.size()))
        {
            _currentOption = VolumeOptions[_currentOptionInt];
        }
    });
}

SoundButton::~SoundButton() = default;

void SoundButton::Awake()
{
    GetChildObject();

    size_t cnt = _volumeBarsFocus.size();
    for (size_t i = 0; i < cnt; ++i)
    {
        if (_volumeBarsFocus[i])
            _volumeBarsFocus[i]->SetActive(false);
        if (_volumeBarsNonFocus[i])
            _volumeBarsNonFocus[i]->SetActive(false);
    }
    cnt = _volumeNumFocus.size();
    for (size_t i = 0; i < cnt; ++i)
    {
        if (_volumeNumFocus[i])
            _volumeNumFocus[i]->SetActive(false);
        if (_volumeNumNonFocus[i])
            _volumeNumNonFocus[i]->SetActive(false);
    }
    // 양옆 화살표 숨기기
    if (_leftArrow)
        _leftArrow->SetActive(false);
    if (_rightArrow)
        _rightArrow->SetActive(false);
    if (_title[1])
        _title[1]->SetActive(false);
    if (_title[0])
        _title[0]->SetActive(true);
    if (_volumeNumNonFocus[_currentVolume])
        _volumeNumNonFocus[_currentVolume]->SetActive(true);
    if (_currentVolume > 0)
    {
        if (_volumeBarsNonFocus[_currentVolume - 1])
            _volumeBarsNonFocus[_currentVolume - 1]->SetActive(true);
    }
    _currentVolume = MaxVolume;
    if ("MasterVolume" == _currentOption)
        _currentVolume = static_cast<int>(UmPreferences.GetMasterVolume() * MaxVolume);
    else if ("BGMVolume" == _currentOption)
        _currentVolume = static_cast<int>(UmPreferences.GetBGMVolume() * MaxVolume);       
    else if ("SFXVolume" == _currentOption)
        _currentVolume = static_cast<int>(UmPreferences.GetSFXVolume() * MaxVolume);
    //_isOptionDirty = true;
    ChangeVolume(0);
}

void SoundButton::Start()
{
    // 관리 매니저 객체
    GameObject* manager = GameObject::Find("PreferencesManager").lock().get();
    if (manager)
    {
        _preferencesManager = manager->GetComponent<PreferencesManager>();
        if (nullptr == _preferencesManager)
            UmLogger.Log(LogLevel::LEVEL_WARNING, "Preferences manager not registered!");
        else
        {
            _preferencesManager->AddPreferencesButton(this);
        }
    }
    else
    {
        UmLogger.Log(LogLevel::LEVEL_WARNING, "Preferences manager not registered!");
    }
    
}

void SoundButton::Reset()
{
    UINavigationComponent::Reset();
    BindInputAction(ControllerButton::DPAD_LEFT, Action::PRESSED, this, &SoundButton::ControlVolumeDown);
    BindInputAction(ControllerButton::DPAD_RIGHT, Action::PRESSED, this, &SoundButton::ControlVolumeUp);
    BindInputAction(ControllerButton::LEFT_THUMB_STICK, Action::PRESSED, this, &SoundButton::ControlVolumeStick);
}
void SoundButton::Update()
{
    if (!_isOptionDirty)
    {
        if (_isVolumeUp)
        {
            ChangeVolume(+1);
            _isVolumeUp = false;
        }
        else if (_isVolumeDown)
        {
            ChangeVolume(-1);
            _isVolumeDown = false;
        }
    }

    UpdateUIForFocus();
    _isOptionDirty = false;
}

void SoundButton::ChangeVolume(const int delta)
{
    // 현재 볼륨 끄기
    if (_volumeNumFocus[_currentVolume])
        _volumeNumFocus[_currentVolume]->SetActive(false);
    if (_volumeNumNonFocus[_currentVolume])
        _volumeNumNonFocus[_currentVolume]->SetActive(false);
    if (_currentVolume > 0)
    {
        if (_volumeBarsFocus[_currentVolume - 1])
            _volumeBarsFocus[_currentVolume - 1]->SetActive(false);
        if (_volumeBarsNonFocus[_currentVolume - 1])
            _volumeBarsNonFocus[_currentVolume - 1]->SetActive(false);
    }

    // 볼륨 값 갱신
    _currentVolume = std::clamp(_currentVolume + delta, 0, MaxVolume);

    // 새로운 볼륨 켜기 (포커스 여부에 따라 분리)
    if (_isFocus)
    {
        if (_volumeNumFocus[_currentVolume])
            _volumeNumFocus[_currentVolume]->SetActive(true);
        if (_currentVolume > 0)
        {
            if (_volumeBarsFocus[_currentVolume - 1])
                _volumeBarsFocus[_currentVolume - 1]->SetActive(true);
        }
        if (_preferencesManager)
        _preferencesManager->SetVolume(_currentOption,_currentVolume);
    }
}

void SoundButton::UpdateUIForFocus()
{
    if (_leftArrow)
        _leftArrow->SetActive(_isFocus);

    if (_rightArrow)
        _rightArrow->SetActive(_isFocus);

    if (_title[true])
        _title[true]->SetActive(_isFocus);

    if (_title[false])
        _title[false]->SetActive(!_isFocus);

    for (size_t i = 0; i < _volumeNumFocus.size(); ++i)
    {
        if (_volumeNumFocus[i])
            _volumeNumFocus[i]->SetActive(_isFocus && i == _currentVolume);

        if (i < _volumeNumNonFocus.size() && _volumeNumNonFocus[i])
            _volumeNumNonFocus[i]->SetActive(!_isFocus && i == _currentVolume);
    }

    for (size_t i = 0; i < _volumeBarsFocus.size(); ++i)
    {
        if (_volumeBarsFocus[i])
            _volumeBarsFocus[i]->SetActive(_isFocus && i < _currentVolume);

        if (i < _volumeBarsNonFocus.size() && _volumeBarsNonFocus[i])
            _volumeBarsNonFocus[i]->SetActive(!_isFocus && i < _currentVolume);
    }
}

void SoundButton::FocusIn(const FocusCallType callType)
{
    Base::FocusIn(callType);

    _isFocus       = true;
    _isOptionDirty = true;
}

void SoundButton::FocusOut(const FocusCallType callType)
{
    UINavigationComponent::FocusOut(callType);

    _isFocus       = false;
    _isOptionDirty = true;
}

void SoundButton::SerializedReflectEvent() 
{
    ReflectFields->CurrentOptionStr = _currentOption;
}

void SoundButton::DeserializedReflectEvent() 
{
    UINavigationComponent::DeserializedReflectEvent();
    _currentOption = ReflectFields->CurrentOptionStr;
}

void SoundButton::ControlVolumeUp(const Input::Controller& controller)
{
    if (!_isFocus)
        return;
    _isOptionDirty = true;
    _isVolumeUp    = true;
    UmAudio.Play("-40000");
}

void SoundButton::ControlVolumeDown(const Input::Controller& controller)
{
    if (!_isFocus)
        return;
    _isOptionDirty = true;
    _isVolumeDown  = true;
    UmAudio.Play("-40000");
}

void SoundButton::ControlVolumeStick(const Input::Controller& controller)
{
    if (!_isFocus)
        return;

    auto bias = controller.GetLeftStickBias();
    if (bias == Input::Controller::StickBias::BIAS_LEFT)
    {
        ControlVolumeDown(controller);
    }
    else if (bias == Input::Controller::StickBias::BIAS_RIGHT)
    {
        ControlVolumeUp(controller);
    }
}
void SoundButton::GetChildObject()
{
    int childCnt = transform->GetChildCount();
    for (int i = 0; i < childCnt; ++i)
    {
        Transform* child = transform->GetChild(i);
        if (nullptr == child)
            continue;
        if (child->gameObject->CompareTag("LeftArrow"))
        {
            _leftArrow = &(child->gameObject);
        }
        else if (child->gameObject->CompareTag("RightArrow"))
        {
            _rightArrow = &(child->gameObject);
        }
        else if (child->gameObject->CompareTag("TitleFocus"))
        {
            _title[1] = &(child->gameObject);
        }
        else if (child->gameObject->CompareTag("TitleNonFocus"))
        {
            _title[0] = &(child->gameObject);
        }
        else if (child->gameObject->CompareTag("VolumeBar"))
        {
            _volumeBar = &(child->gameObject);
        }
        else if (child->gameObject->CompareTag("VolumeNum"))
        {
            _volumeNum = &(child->gameObject);
        }
    }

    childCnt = _volumeBar->transform->GetChildCount();
    for (int i = 0; i < childCnt; ++i)
    {
        Transform* child = _volumeBar->transform->GetChild(i);
        if (nullptr == child)
            continue;
        if (child->gameObject->CompareTag("BarFocus"))
            _volumeBarsFocus.push_back(&(child->gameObject));
        else if (child->gameObject->CompareTag("BarNonFocus"))
            _volumeBarsNonFocus.push_back(&(child->gameObject));
    }

    childCnt = _volumeNum->transform->GetChildCount();
    for (int i = 0; i < childCnt; ++i)
    {
        Transform* child = _volumeNum->transform->GetChild(i);
        if (nullptr == child)
            continue;
        if (child->gameObject->CompareTag("NumFocus"))
            _volumeNumFocus.push_back(&(child->gameObject));
        else if (child->gameObject->CompareTag("NumNonFocus"))
            _volumeNumNonFocus.push_back(&(child->gameObject));
    }
}
