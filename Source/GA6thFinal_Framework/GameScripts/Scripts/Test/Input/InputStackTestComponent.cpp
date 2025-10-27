#include "pchScripts.h"
#include "InputStackTestComponent.h"
#include "UI/Elements/Image/ImageElement.h"

UMREAL_COMPONENT(InputStackTestComponent)

InputStackTestComponent::InputStackTestComponent() = default;
InputStackTestComponent::~InputStackTestComponent() = default;

void InputStackTestComponent::Awake() 
{
    BindInputAction(ControllerButton::A, Action::PRESSED, this, &InputStackTestComponent::OnPush);
    BindInputAction(ControllerButton::B, Action::PRESSED, this, &InputStackTestComponent::OnPop);
}

void InputStackTestComponent::Start() 
{
    PushInputLayer();
    auto& image = AddComponent<ImageElement>();
    image.SetImage(_image);
}

void InputStackTestComponent::OnPush(const Input::Controller&) 
{
    auto object = NewGameObject<GameObject>("Input Layer");
    if (object)
    {
        object->AddComponent<InputStackTestComponent>();
    }  
}

void InputStackTestComponent::OnPop(const Input::Controller&) 
{
    PopInputLayer();
    GameObject::Destroy(gameObject);
}
