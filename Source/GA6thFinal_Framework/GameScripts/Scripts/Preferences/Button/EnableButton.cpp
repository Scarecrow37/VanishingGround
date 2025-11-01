#include "pchScripts.h"
#include "EnableButton.h"
#include "PreferencesButton.h"
#include "Scripts/Preferences/PreferencesManager.h"
#include "Scripts/UI/Elements/Image/ImageElement.h"

UMREAL_COMPONENT(EnableButton)

EnableButton::EnableButton()
{
    OnFocus.SetInputAutoEvent([this]() {
        if (ImGui::BeginDragDropTarget())
        {
            if (const ImGuiPayload* payLoad = ImGui::AcceptDragDropPayload(DragDropAsset::KEY))
            {

                const DragDropAsset::Data* data = static_cast<DragDropAsset::Data*>(payLoad->Data);
                if (const auto extension = data->GetPath().extension(); extension == L".png" || extension == L".jpeg")
                {
                    _onFocusImage              = data->GetGuid();
                    ReflectFields->OnFocusGuid = _onFocusImage.string();
                    auto image                 = GetComponent<ImageElement>();
                    if (nullptr != image)
                    {
                        image->SetImage(_onFocusImage);
                    }
                }
            }
            ImGui::EndDragDropTarget();
        }
    });

    OnNonFocus.SetInputAutoEvent([this]() {
        if (ImGui::BeginDragDropTarget())
        {
            if (const ImGuiPayload* payLoad = ImGui::AcceptDragDropPayload(DragDropAsset::KEY))
            {

                const DragDropAsset::Data* data = static_cast<DragDropAsset::Data*>(payLoad->Data);
                if (const auto extension = data->GetPath().extension(); extension == L".png" || extension == L".jpeg")
                {
                    _onNonFocusImage              = data->GetGuid();
                    ReflectFields->OnNonFocusGuid = _onNonFocusImage.string();
                    auto image                    = GetComponent<ImageElement>();
                    if (nullptr != image)
                        image->SetImage(_onNonFocusImage);
                }
            }
            ImGui::EndDragDropTarget();
        }
    });

    OffFocus.SetInputAutoEvent([this]() {
        if (ImGui::BeginDragDropTarget())
        {
            if (const ImGuiPayload* payLoad = ImGui::AcceptDragDropPayload(DragDropAsset::KEY))
            {

                const DragDropAsset::Data* data = static_cast<DragDropAsset::Data*>(payLoad->Data);
                if (const auto extension = data->GetPath().extension(); extension == L".png" || extension == L".jpeg")
                {
                    _offFocusImage              = data->GetGuid();
                    ReflectFields->OffFocusGuid = _offFocusImage.string();
                    auto image                  = GetComponent<ImageElement>();
                    if (nullptr != image)
                        image->SetImage(_offFocusImage);
                }
            }
            ImGui::EndDragDropTarget();
        }
    });

    OffNonFocus.SetInputAutoEvent([this]() {
        if (ImGui::BeginDragDropTarget())
        {
            if (const ImGuiPayload* payLoad = ImGui::AcceptDragDropPayload(DragDropAsset::KEY))
            {

                const DragDropAsset::Data* data = static_cast<DragDropAsset::Data*>(payLoad->Data);
                if (const auto extension = data->GetPath().extension(); extension == L".png" || extension == L".jpeg")
                {
                    _offNonFocusImage              = data->GetGuid();
                    ReflectFields->OffNonFocusGuid = _offNonFocusImage.string();
                    auto image                     = GetComponent<ImageElement>();
                    if (nullptr != image)
                        image->SetImage(_offNonFocusImage);
                }
            }
            ImGui::EndDragDropTarget();
        }
    });

    CurrentOption.SetInputAutoEvent([this]() {
        if (ImGui::Combo("GraphicsOptions", &_currentOptionInt, GraphicsOptions.data(), (int)GraphicsOptions.size()))
        {
            _currentOption = GraphicsOptions[_currentOptionInt];
        }
    });
}
EnableButton::~EnableButton() = default;

void EnableButton::Awake()
{
    UmSceneManager.ResourceManager.RequestTextureResource(this, _onFocusImage, []() {});
    UmSceneManager.ResourceManager.RequestTextureResource(this, _onNonFocusImage, []() {});
    UmSceneManager.ResourceManager.RequestTextureResource(this, _offFocusImage, []() {});
    UmSceneManager.ResourceManager.RequestTextureResource(this, _offNonFocusImage, []() {});
    // 자주사용할 게임오브젝트 포인터
    GetChildObject();

    // 초기값
    if (!_onNonFocusImage.IsNull())
    {
        bool option = true;
        if ("SSR" == _currentOption)
            option = UmPreferences.IsSSR();
        else if ("SSAO" == _currentOption)
            option = UmPreferences.IsSSAO();
        else if ("Bloom" == _currentOption)
            option = UmPreferences.IsBloom();
        else if ("VolumetricFog" == _currentOption)
            option = UmPreferences.IsVolumFog();
        
        auto image = GetComponent<ImageElement>();
        if (nullptr != image)
        {
            if (option)
            {
                image->SetImage(_onNonFocusImage);
                _isOptionOn = option;
            }
            else
            {
                image->SetImage(_offNonFocusImage);
                _isOptionOn = option;
            }
        }
    }

    // 양옆 화살표 숨기기
    if (_leftArrow)
        _leftArrow->SetActive(false);
    if (_rightArrow)
        _rightArrow->SetActive(false);

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
void EnableButton::Reset()
{
    UINavigationComponent::Reset();
    BindInputAction(ControllerButton::DPAD_RIGHT, Action::PRESSED, this, &EnableButton::ChangeOptionDpad);
    BindInputAction(ControllerButton::DPAD_LEFT, Action::PRESSED, this, &EnableButton::ChangeOptionDpad);
    BindInputAction(ControllerButton::LEFT_THUMB_STICK, Action::PRESSED, this, &EnableButton::ChangeOptionStick);
}

void EnableButton::Update() 
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
            if (_isOptionOn)
            {
                auto image = GetComponent<ImageElement>();
                if (nullptr != image)
                    image->SetImage(_onFocusImage);
                ChangeOption();
            }
            else
            {
                auto image = GetComponent<ImageElement>();
                if (nullptr != image)
                    image->SetImage(_offFocusImage);
                ChangeOption();
            }
        }
        else
        {
            FocusPref(false);
            if (_leftArrow)
                _leftArrow->SetActive(false);
            if (_rightArrow)
                _rightArrow->SetActive(false);

            if (_isOptionOn)
            {
                auto image = GetComponent<ImageElement>();
                if (nullptr != image)
                    image->SetImage(_onNonFocusImage);
                ChangeOption();
            }
            else
            {
                auto image = GetComponent<ImageElement>();
                if (nullptr != image)
                    image->SetImage(_offNonFocusImage);
                ChangeOption();
            }
        }
        _isOptionDirty = false;
    }
}

void EnableButton::OnEnable()
{
    if (_preferencesManager)
    {
        bool preferencesIsOpen = _preferencesManager->IsOpen();
        if ("SSR" == _currentOption && !preferencesIsOpen)
        {
            Focus();
        }
    }
}

void EnableButton::FocusIn(const FocusCallType callType)
{
    Base::FocusIn(callType);

    _isFocus   = true;
    _isOptionDirty = true;
}

void EnableButton::FocusOut(const FocusCallType callType)
{
    UINavigationComponent::FocusOut(callType);

    _isFocus   = false;
    _isOptionDirty = true;
}

void EnableButton::Submit()
{
    Base::Submit();
}

void EnableButton::SerializedReflectEvent()
{
    ReflectFields->OnFocusGuid      = _onFocusImage.string();
    ReflectFields->OnNonFocusGuid   = _onNonFocusImage.string();
    ReflectFields->OffFocusGuid     = _offFocusImage.string();
    ReflectFields->OffNonFocusGuid  = _offNonFocusImage.string();
    ReflectFields->CurrentOptionStr = _currentOption;
}

void EnableButton::DeserializedReflectEvent()
{
    UINavigationComponent::DeserializedReflectEvent();
    _onFocusImage     = ReflectFields->OnFocusGuid;
    _onNonFocusImage  = ReflectFields->OnNonFocusGuid;
    _offFocusImage    = ReflectFields->OffFocusGuid;
    _offNonFocusImage = ReflectFields->OffNonFocusGuid;
    _currentOption    = ReflectFields->CurrentOptionStr;
}

void EnableButton::ChangeOptionDpad(const Input::Controller& controller)
{
    if (_isFocus)
    {
        _isOptionOn = !_isOptionOn;
        _isOptionDirty = true;
        UmAudio.Play("-40000");
    }
}

void EnableButton::ChangeOptionStick(const Input::Controller& controller)
{
    if (_isFocus)
    {
        if (Input::Controller::StickBias::BIAS_LEFT == controller.GetLeftStickBias() ||
            Input::Controller::StickBias::BIAS_RIGHT == controller.GetLeftStickBias())
        {
            _isOptionOn = !_isOptionOn;
            _isOptionDirty = true;
            UmAudio.Play("-40000");
        }
    }
}

void EnableButton::FocusPref(const bool isfocus)
{
    if (nullptr == _pref)
    {
        GetChildObject();
    }
    auto prefButton = _pref->GetComponent<PreferencesButton>();
    if (nullptr != prefButton)
        prefButton->OnFocus(isfocus);
}

void EnableButton::GetChildObject()
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
}

void EnableButton::ChangeOption()
{
    if (_preferencesManager)
        _preferencesManager->SetGraphicsOptions(_currentOption, _isOptionOn);
}
