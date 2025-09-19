#include "pchScripts.h"
#include "SoundButton.h"
#include "Scripts/Preferences/PreferencesManager.h"

UMREAL_COMPONENT(SoundButton)

SoundButton::SoundButton()  = default;
SoundButton::~SoundButton() = default;

void SoundButton::Awake() 
{
    GetChildObject();

    size_t cnt = _volumeBarsFocus.size();
    for (size_t i = 0; i < cnt; ++i)
    {
        _volumeBarsFocus[i]->SetActive(false);
        _volumeBarsNonFocus[i]->SetActive(false);
    }
    cnt = _volumeNumFocus.size();
    for (size_t i = 0; i < cnt; ++i)
    {
        _volumeNumFocus[i]->SetActive(false);
        _volumeNumNonFocus[i]->SetActive(false);
    }
    // 양옆 화살표 숨기기
    _leftArrow->SetActive(false);
    _rightArrow->SetActive(false);
    _title[1]->SetActive(false);
    _title[0]->SetActive(true);
    _volumeNumNonFocus[_currentVolume]->SetActive(true);
    if (_currentVolume > 0)
    {
        _volumeBarsNonFocus[_currentVolume - 1]->SetActive(true);
    }
}

void SoundButton::Start() 
{
    // 관리 매니저 객체
    GameObject* manager = GameObject::Find("PreferencesManager").lock().get();
    _preferencesManager = manager->GetComponent<PreferencesManager>();
    if (nullptr == _preferencesManager)
        UmLogger.Log(LogLevel::LEVEL_WARNING, "Preferences manager not registered!");
}

void SoundButton::Reset()
{
    UINavigationComponent::Reset();
    BindInputAction(ControllerButton::DPAD_LEFT, Action::PRESSED, this, &SoundButton::ControlVolumeDown);
    BindInputAction(ControllerButton::DPAD_RIGHT, Action::PRESSED, this, &SoundButton::ControlVolumeUp);
    BindInputAction(ControllerButton::LEFT_THUMB_STICK, Action::PRESSED, this, &SoundButton::ControlVolumeStick);
}

void SoundButton::Update() {}

void SoundButton::FocusIn() 
{
    UINavigationComponent::FocusIn();

    _isFocus = true;

    _leftArrow->SetActive(true);
    _rightArrow->SetActive(true);

    _title[true]->SetActive(true);   
    _title[false]->SetActive(false);

    _volumeNumFocus[_currentVolume]->SetActive(true);
    _volumeNumNonFocus[_currentVolume]->SetActive(false);

    if (_currentVolume > 0)
    {
        _volumeBarsFocus[_currentVolume - 1]->SetActive(true);
        _volumeBarsNonFocus[_currentVolume - 1]->SetActive(false);
    }
}

void SoundButton::FocusOut() 
{
    UINavigationComponent::FocusOut();

    _isFocus = false;

    _leftArrow->SetActive(false);
    _rightArrow->SetActive(false);

    _title[true]->SetActive(false);
    _title[false]->SetActive(true);

    _volumeNumFocus[_currentVolume]->SetActive(false);
    _volumeNumNonFocus[_currentVolume]->SetActive(true);

    if (_currentVolume > 0)
    {
        _volumeBarsFocus[_currentVolume - 1]->SetActive(false);
        _volumeBarsNonFocus[_currentVolume - 1]->SetActive(true);
    }
}

void SoundButton::ControlVolumeUp(const Input::Controller& controller)
{
    if (!_isFocus)
        return;

    _volumeNumFocus[_currentVolume]->SetActive(false);

    if (_currentVolume > 0)
        _volumeBarsFocus[_currentVolume - 1]->SetActive(false);

    if (_currentVolume < MaxVolume)
        _currentVolume++;

    _volumeNumFocus[_currentVolume]->SetActive(true);

    if (_currentVolume > 0)
        _volumeBarsFocus[_currentVolume - 1]->SetActive(true);
}

void SoundButton::ControlVolumeDown(const Input::Controller& controller) 
{
    if (!_isFocus)
        return;

    _volumeNumFocus[_currentVolume]->SetActive(false);

    if (_currentVolume > 0)
        _volumeBarsFocus[_currentVolume - 1]->SetActive(false);

    if (_currentVolume > 0)
        _currentVolume--;

    _volumeNumFocus[_currentVolume]->SetActive(true);

    if (_currentVolume > 0)
        _volumeBarsFocus[_currentVolume - 1]->SetActive(true);
}

void SoundButton::ControlVolumeStick(const Input::Controller& controller)
{
    if (!_isFocus)
        return;

    auto bias = controller.GetLeftStickBias();
    if (bias == Input::Controller::StickBias::LEFT)
    {
        ControlVolumeDown(controller);
    }
    else if (bias == Input::Controller::StickBias::RIGHT)
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
