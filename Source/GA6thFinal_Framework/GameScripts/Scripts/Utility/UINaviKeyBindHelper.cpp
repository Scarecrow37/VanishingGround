#include "pchScripts.h"
#include "UINaviKeyBindHelper.h"

UMREAL_COMPONENT(UINaviKeyBindHelper)

NavigationKey UINaviKeyBindHelper::MakeDpadKey(BindType type)
{
    using namespace Input;
    NavigationKey key;
    switch (type)
    {
    case UINaviKeyBindHelper::BindType::LEFT:
        key.ButtonType = Controller::Button::DPAD_LEFT;
        break;
    case UINaviKeyBindHelper::BindType::RIGHT:
        key.ButtonType = Controller::Button::DPAD_RIGHT;
        break;
    case UINaviKeyBindHelper::BindType::UP:
        key.ButtonType = Controller::Button::DPAD_UP;
        break;
    case UINaviKeyBindHelper::BindType::DOWN:
        key.ButtonType = Controller::Button::DPAD_DOWN;
        break;
    default:
        return key;
    }
    key.Name = Input::Controller::GetButtonName(key.ButtonType);
    key.Bias = Controller::StickBias::UNBIASED;
    return key;
}

NavigationKey UINaviKeyBindHelper::MakeThumbStickKey(BindType type)
{
    using namespace Input;
    NavigationKey key;
    key.ButtonType = Controller::Button::LEFT_THUMB_STICK;
    switch (type)
    {
    case UINaviKeyBindHelper::BindType::LEFT:
        key.Bias = Controller::StickBias::BIAS_LEFT;
        break;
    case UINaviKeyBindHelper::BindType::RIGHT:
        key.Bias = Controller::StickBias::BIAS_RIGHT;
        break;
    case UINaviKeyBindHelper::BindType::UP:
        key.Bias = Controller::StickBias::BIAS_UP;
        break;
    case UINaviKeyBindHelper::BindType::DOWN:
        key.Bias = Controller::StickBias::BIAS_DOWN;
        break;
    default:
        return key;
    }
    key.Name = Input::Controller::GetButtonName(key.ButtonType);
    key.Name += " ";
    key.Name += Input::Controller::GetStickBiasName(key.Bias);
    return key;
}

UINaviKeyBindHelper::UINaviKeyBindHelper()  = default;
UINaviKeyBindHelper::~UINaviKeyBindHelper() = default;

void UINaviKeyBindHelper::FindNavi() 
{
    if (auto navi = gameObject->GetComponentDynamic<UINavigationComponent>())
    {
        _navi = navi->GetWeakPtr();
    }
}

void UINaviKeyBindHelper::BindNavi(UINavigationComponent* navi, BindType type, int navigationID) 
{
    if (navi && 0 < navigationID)
    {
        NavigationKey dpad = MakeDpadKey(type);
        NavigationKey stick = MakeThumbStickKey(type);    
        navi->AddNavigationRoute(dpad, navigationID);
        navi->AddNavigationRoute(stick, navigationID);
    }
}

void UINaviKeyBindHelper::Added() 
{
    FindNavi();
}

void UINaviKeyBindHelper::ImGuiDrawPropertysEvent() 
{
    if (ImGui::TreeNode("Bind Helper"))
    {
        if (ImGui::Button("Find Navi"))
        {
            FindNavi();
        }
        ImGui::Separator();
        if (auto component = _navi.lock())
        {
            UINavigationComponent* navi = static_cast<UINavigationComponent*>(component.get());
            static int leftID = INVALID_NAVIGATION_ID;
            if(ImGui::InputInt("Left navigation ID", &leftID))
            {
                leftID = std::clamp(leftID, 0, std::numeric_limits<int>::max());
            }
            static int rightID = INVALID_NAVIGATION_ID;
            if (ImGui::InputInt("Right navigation ID", &rightID))
            {
                rightID = std::clamp(rightID, 0, std::numeric_limits<int>::max());
            }
            static int upID = INVALID_NAVIGATION_ID;
            if (ImGui::InputInt("Up navigation ID", &upID))
            {
                upID = std::clamp(upID, 0, std::numeric_limits<int>::max());
            }
            static int downID = INVALID_NAVIGATION_ID;
            if (ImGui::InputInt("Down navigation ID", &downID))
            {
                downID = std::clamp(downID, 0, std::numeric_limits<int>::max());
            }
            if (ImGui::Button("+"))
            {
                BindNavi(navi, BindType::LEFT, leftID);
                BindNavi(navi, BindType::RIGHT, rightID);
                BindNavi(navi, BindType::UP, upID);
                BindNavi(navi, BindType::DOWN, downID);
                GameObject::Destroy(this);
            }
        }
        ImGui::TreePop();
    }
}
