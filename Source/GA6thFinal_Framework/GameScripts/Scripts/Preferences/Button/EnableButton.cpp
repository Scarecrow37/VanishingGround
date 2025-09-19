#include "pchScripts.h"
#include "EnableButton.h"
#include "PreferencesButton.h"
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
                    auto image            = GetComponent<ImageElement>();
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
                    auto image             = GetComponent<ImageElement>();
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
                    ReflectFields->OffNonFocusGuid= _offNonFocusImage.string();
                    auto image                 = GetComponent<ImageElement>();
                    if (nullptr != image)
                        image->SetImage(_offNonFocusImage);
                }
            }
            ImGui::EndDragDropTarget();
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

    // 이미지 기본값
    if (!_onNonFocusImage.IsNull())
    {
        auto image = GetComponent<ImageElement>();
        if (nullptr != image)
            image->SetImage(_onNonFocusImage);
        _isOptionOn = true;
    }

    // 양옆 화살표 숨기기
    _leftArrow->SetActive(false);
    _rightArrow->SetActive(false);

}

void EnableButton::Start() {}

void EnableButton::Reset()
{
    UINavigationComponent::Reset();
    BindInputAction(ControllerButton::DPAD_RIGHT, Action::PRESSED, this, &EnableButton::ChangeOptionDpad);
    BindInputAction(ControllerButton::DPAD_LEFT, Action::PRESSED, this, &EnableButton::ChangeOptionDpad);
    BindInputAction(ControllerButton::LEFT_THUMB_STICK, Action::PRESSED, this, &EnableButton::ChangeOptionStick);
}

void EnableButton::Update() {}

void EnableButton::FocusIn()
{
    UINavigationComponent::FocusIn();

    _isFocus   = true;
    auto image = GetComponent<ImageElement>();
    if (nullptr != image)
    {
        if (_isOptionOn)
        {
            image->SetImage(_onFocusImage);
        }
        else
        {
            image->SetImage(_offFocusImage);
        }
    }
    FocusPref(true);
    _leftArrow->SetActive(true);
    _rightArrow->SetActive(true);
}

void EnableButton::FocusOut() 
{
    UINavigationComponent::FocusOut();

    _isFocus = false;
    auto image = GetComponent<ImageElement>();
    if (nullptr != image)
    {
        if (_isOptionOn)
        {
            image->SetImage(_onNonFocusImage);
        }
        else
        {
            image->SetImage(_offNonFocusImage);
        }
    }
    FocusPref(false);
    _leftArrow->SetActive(false);
    _rightArrow->SetActive(false);
}

void EnableButton::Submit() {}

void EnableButton::SerializedReflectEvent()
{
    ReflectFields->OnFocusGuid = _onFocusImage.string();
    ReflectFields->OnNonFocusGuid = _onNonFocusImage.string();
    ReflectFields->OffFocusGuid = _offFocusImage.string();
    ReflectFields->OffNonFocusGuid = _offNonFocusImage.string();
}

void EnableButton::DeserializedReflectEvent() 
{
    UINavigationComponent::DeserializedReflectEvent();
    _onFocusImage = ReflectFields->OnFocusGuid;
    _onNonFocusImage = ReflectFields->OnNonFocusGuid;
    _offFocusImage = ReflectFields->OffFocusGuid;
    _offNonFocusImage = ReflectFields->OffNonFocusGuid;
}

void EnableButton::ChangeOptionDpad(const Input::Controller& controller)
{
    if (_isFocus)
    {
        _isOptionOn = !_isOptionOn;
        if (_isOptionOn)
        {
            auto image = GetComponent<ImageElement>();
            if (nullptr != image)
                image->SetImage(_onFocusImage);
        }
        else
        {
            auto image = GetComponent<ImageElement>();
            if (nullptr != image)
                image->SetImage(_offFocusImage);
        }
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
            if (_isOptionOn)
            {
                auto image = GetComponent<ImageElement>();
                if (nullptr != image)
                    image->SetImage(_onFocusImage);
            }
            else
            {
                auto image = GetComponent<ImageElement>();
                if (nullptr != image)
                    image->SetImage(_offFocusImage);
            }
        }
    }
}

void EnableButton::FocusPref(bool isfocus) 
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
