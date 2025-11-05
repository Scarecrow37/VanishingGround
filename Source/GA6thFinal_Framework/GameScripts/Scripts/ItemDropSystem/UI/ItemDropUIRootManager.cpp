#include "pchScripts.h"
#include "ItemDropUIRootManager.h"
#include "ItemDropSystem/UI/ArtifactUIManager.h"
#include "ItemDropSystem/ItemDropSystem.h"
#include "RevelationSystem/RevelationSystem.h"
#include "ExcelDataSystem/ExcelDataSystem.h"
#include "ItemDropSystem/UI/ItemInfoUIManager.h"
#include "ItemDropSystem/UINavi/RestartStageNavi.h"
#include "ItemDropSystem/UINavi/ArtifactButtonNavi.h"
#include "ItemDropSystem/UI/WeaponChangeUIManager.h"
#include "ItemDropSystem/UI/EraseRevelationUIManager.h"
#include "ItemDropSystem/UINavi/ReturnToMapNavi.h"
#include "UI/Elements/Text/TextElement.h"
#include "UI/Elements/Image/ImageElement.h"
#include "Preferences/PreferencesManager.h"
#include "Inventory/UI/InventoryUIManager.h"
#include "UI/Panels/Description/DescriptionPanel.h"
#include "Map/MapManager.h"
#include "Map/Stage.h"

UMREAL_COMPONENT(ItemDropUIRootManager)

ItemDropUIRootManager::ItemDropUIRootManager()
{
    _lastInputDir = InputDir::IDLE;
}

ItemDropUIRootManager::~ItemDropUIRootManager()
{
  
}

void ItemDropUIRootManager::AutoFocus(bool checkInputDir)
{
    if (ArtifactUIManager* artifactUI = ArtifactUI)
    {
        if (false == artifactUI->IsObtainActive())
        {
            size_t startIndex = ArtifactButtonNavi::GetLastFocusIndex();
            bool   revers     = false;
            // 입력 체크에 따른 보정
            if (checkInputDir)
            {
                constexpr size_t horizontalDamp = 2;
                constexpr size_t verticalDamp   = 1;

                switch (_lastInputDir)
                {
                case ItemDropUIRootManager::InputDir::LEFT:
                    startIndex = horizontalDamp <= startIndex ? startIndex - horizontalDamp : 0;
                    revers     = true;
                    break;
                case ItemDropUIRootManager::InputDir::RIGHT:
                    startIndex = startIndex + horizontalDamp;
                    break;
                case ItemDropUIRootManager::InputDir::UP:
                    startIndex = verticalDamp <= startIndex ? startIndex - verticalDamp : 0;
                    revers     = true;
                    break;
                case ItemDropUIRootManager::InputDir::DOWN:
                    startIndex = startIndex + verticalDamp;
                    break;
                case ItemDropUIRootManager::InputDir::IDLE:
                default:
                    break;
                }
                _lastInputDir = ItemDropUIRootManager::InputDir::IDLE;
            }
            startIndex = std::min(startIndex, ARTIFACT_DROP_COUNT);

            // 포커스 가능한 UI로 설정
            if (false == revers)
            {
                for (size_t i = startIndex; i < ARTIFACT_DROP_COUNT; ++i)
                {
                    if (artifactUI->FocusNavi(i))
                    {
                        _isFocusArtifactNavi = true;
                        return;
                    }
                }
            }
            else
            {
                size_t i = startIndex;
                while (i < ARTIFACT_DROP_COUNT)
                {
                    if (artifactUI->FocusNavi(i))
                    {
                        _isFocusArtifactNavi = true;
                        return;
                    }
                    if (i == 0)
                    {
                        break;
                    }
                    --i;
                }

                for (size_t i = startIndex; i < ARTIFACT_DROP_COUNT; ++i)
                {
                    if (artifactUI->FocusNavi(i))
                    {
                        _isFocusArtifactNavi = true;
                        return;
                    }
                }
            }
        }
    }

    //없으면 RestartButton으로 포커스 설정
    if (auto restartNavi = _restartNavi.lock())
    {
        if (restartNavi->Enable)
        {
            restartNavi->Focus();
        }
        else if (auto returnToMapNavi = _returnToMapNavi.lock())
        {
            returnToMapNavi->Focus();
        }
        ArtifactButtonNavi::LastFocusIndex = ARTIFACT_DROP_COUNT + 1;
        _isFocusArtifactNavi               = false;
    } 
}

void ItemDropUIRootManager::UpdateStroy() 
{
    if (MapManager* mapManager = SingletonComponent<MapManager>::GetInstance())
    {
        if (Stage* stage = mapManager->GetCurrentSelectedStage())
        {
            int mainlevel   = stage->MainLevel;
            int subLevel    = stage->SubLevel;
            int battleCount = 0;
            if (ItemDropSystem* dropSystem = SingletonComponent<ItemDropSystem>::GetInstance())
            {
                battleCount = dropSystem->StageClearCount;
            }

            std::string stageID = "211";
            stageID += std::to_string(mainlevel);
            stageID += std::to_string(subLevel);
            stageID += std::to_string(battleCount);

            if (ExcelDataSystem* system = SingletonComponent<ExcelDataSystem>::GetInstance())
            {
                if (auto dataBase = system->FindExcelDataBase(u8"스토리 목록"))
                {
                    size_t rowIndex = dataBase->FindRowIndex((const char8_t*)stageID.c_str(), u8"Stage ID");
                    if (rowIndex != dataBase->FIND_INDEX_FAIL)
                    {
                        std::string_view data = dataBase->FIND_STR_FAIL;
                        data = dataBase->FindData(rowIndex, u8"Journal Name Real");
                        if (data != dataBase->FIND_STR_FAIL)
                        {
                            if (auto title = _stroyPanel.Title.lock())
                            {
                                title->Text = data.data();
                            }
                        }
                        data = dataBase->FindData(rowIndex, u8"Journal Description Real");
                        if (data != dataBase->FIND_STR_FAIL)
                        {
                            if (auto stroy = _stroyPanel.Description.lock())
                            {
                                stroy->Description = data.data();
                            }
                        }
                    }
                }
            }
        }
    }
}

void ItemDropUIRootManager::DeserializedReflectEvent()
{
    
}

void ItemDropUIRootManager::ImGuiDrawPropertysEvent()
{
    auto CheckWeakPtrText = [](const auto& weakPtr) 
    {
        if (auto ptr = weakPtr.lock())
        {
            ImGui::Text("ArtifactUIManager valid");
        }
        else
        {
            ImGui::Text("ArtifactUIManager nullptr");
        }
    };

    if (ImGui::TreeNode("Debug"))
    {
        CheckWeakPtrText(_artifactUIManager); 
        CheckWeakPtrText(_itemInfoUIManager);
        ImGui::TreePop();
    }
}

void ItemDropUIRootManager::Reset() 
{
    _singletonComponent.SetSingleTon();
    if (true == UmCore->IsPlay())
    {
        gameObject->ActiveSelf = true;
    }
}

void ItemDropUIRootManager::Awake()
{
    Base::Awake();
    if (_singletonComponent.TrySingleTon())
    {
        gameObject->AddTag(ItemDropUIRootManager::TAG);
        BindInputAction(ControllerButton::DPAD_LEFT, Action::PRESSED, this, &ItemDropUIRootManager::OnDpadLeft);
        BindInputAction(ControllerButton::DPAD_RIGHT, Action::PRESSED, this, &ItemDropUIRootManager::OnDpadRight);
        BindInputAction(ControllerButton::DPAD_UP, Action::PRESSED, this, &ItemDropUIRootManager::OnDpadUp);
        BindInputAction(ControllerButton::DPAD_DOWN, Action::PRESSED, this, &ItemDropUIRootManager::OnDpadDown);
        BindInputAction(ControllerButton::LEFT_THUMB_STICK, Action::PRESSED, this, &ItemDropUIRootManager::OnLeftTumbStickDown);
    }
}

void ItemDropUIRootManager::Start() 
{
    if (_singletonComponent.IsSingleTon())
    {
        if (auto artifactUI = GameObject::FindWithTag(ArtifactUIManager::TAG).lock())
        {
            if (ArtifactUIManager* component = artifactUI->GetComponent<ArtifactUIManager>())
            {
                auto weakComponent = component->GetWeakPtr();
                _artifactUIManager = std::static_pointer_cast<ArtifactUIManager>(weakComponent.lock());
            }
        }
        if (auto itemInfoUI = transform->FindWithTag(ItemInfoUIManager::TAG))
        {
            if (ItemInfoUIManager* component = itemInfoUI->gameObject->GetComponent<ItemInfoUIManager>())
            {
                auto weakComponent = component->GetWeakPtr();
                _itemInfoUIManager = std::static_pointer_cast<ItemInfoUIManager>(weakComponent.lock());
            }
        }
        if (auto weaponChangeUI = GameObject::FindWithTag(WeaponChangeUIManager::TAG).lock())
        {
            if (WeaponChangeUIManager* component = weaponChangeUI->GetComponent<WeaponChangeUIManager>())
            {
                auto weakComponent = component->GetWeakPtr();
                _weaponChangeUIManager = std::static_pointer_cast<WeaponChangeUIManager>(weakComponent.lock());
            }
        }
        if (auto eraseRevelationObject = GameObject::FindWithTag(EraseRevelationUIManager::TAG).lock())
        {
            if (EraseRevelationUIManager* component = eraseRevelationObject->GetComponent<EraseRevelationUIManager>())
            {
                auto weakComponent = component->GetWeakPtr();
                _eraseRevelationUIManager = std::static_pointer_cast<EraseRevelationUIManager>(weakComponent.lock());
            }
        }
        if (auto restartButtonObject = GameObject::FindWithTag(RestartStageNavi::TAG).lock())
        {
            if (auto navi = restartButtonObject->GetComponent<RestartStageNavi>())
            {
                auto component = navi->GetWeakPtr().lock();
                _restartNavi   = std::static_pointer_cast<RestartStageNavi>(component);
            }
        }
        if (auto returnToMapNaviObject = GameObject::FindWithTag(ReturnToMapNavi::TAG).lock())
        {
            if (auto navi = returnToMapNaviObject->GetComponent<ReturnToMapNavi>())
            {
                auto component = navi->GetWeakPtr().lock();
                _returnToMapNavi = std::static_pointer_cast<ReturnToMapNavi>(component);
            }
        }
        
        if (Transform* stroyTransform = transform->Find("Stroy Panel"))
        {
            Transform::ForeachBFS(*stroyTransform, [this](Transform* curr)
            {
                GameObject& object = curr->gameObject;
                if (object.CompareTag("Title"))
                {
                    if (TextElement* text = object.GetComponent<TextElement>())
                    {
                        _stroyPanel.Title = text->GetWeakPtrAs<TextElement>();
                    }      
                }
                else if (object.CompareTag("Description"))
                {
                    if (DescriptionPanel* descriptionPanel = object.GetComponent<DescriptionPanel>())
                    {
                        _stroyPanel.Description = descriptionPanel->GetWeakPtrAs<DescriptionPanel>();
                    }
                }
                else if (object.CompareTag("Story Image"))
                {
                    if (ImageElement* image = object.GetComponent<ImageElement>())
                    {
                        _stroyPanel.Image = image->GetWeakPtrAs<ImageElement>();
                    }
                }
            });
        }
        gameObject->ActiveSelf = false;
    }
}

void ItemDropUIRootManager::Update() 
{
    UpdateAutoFocus();
    Debugger db;
    db([this]() 
    {
        using namespace u8_literals;
        if (gameObject->ActiveInHierarchy)
        {
            if (ImGui::TreeNode("Stage Clear UI Button"))
            {
                if (ImGui::Button(u8"다음 전투"_c_str))
                {
                    if (auto navi = _restartNavi.lock())
                    {
                        navi->Submit();
                    }
                }
                if (ImGui::Button(u8"떠난다"_c_str))
                {
                    if (auto navi = _returnToMapNavi.lock())
                    {
                        navi->Submit();
                    }
                }
                ImGui::TreePop();
            }
        }
    });
}

void ItemDropUIRootManager::LateUpdate() 
{
    Base::LateUpdate();
    _lastInputDir = InputDir::IDLE;
}

void ItemDropUIRootManager::UpdateAutoFocus() 
{
    if (_isFocusInput)
    {
        WeaponChangeUIManager*    weaponChaingUI     = WeaponChangeUI;
        EraseRevelationUIManager* eraseRevelationUI  = EraseRevelationUI;
        InventoryUIManager*       inventoryUI        = SingletonComponent<InventoryUIManager>::GetInstance();
        PreferencesManager*       preferencesManager = SingletonComponent<PreferencesManager>::GetInstance();

        if (weaponChaingUI)
        {
            if (weaponChaingUI->gameObject->ActiveInHierarchy)
            {
                goto label_end;
            }
        }
        if (eraseRevelationUI)
        {
            if (eraseRevelationUI->gameObject->ActiveInHierarchy)
            {
                goto label_end;
            }
        }
        if (inventoryUI)
        {
            if (inventoryUI->gameObject->ActiveInHierarchy)
            {
                goto label_end;
            }
        }
        if (preferencesManager)
        {
            if (preferencesManager->IsOpen())
            {
                goto label_end;
            }
        }

        AutoFocus(true);
label_end:
        _isFocusInput = false;
    }
}

void ItemDropUIRootManager::OnDpadLeft(const Input::Controller&) 
{
    if (EnableInHierarchy)
    {
        _lastInputDir = InputDir::LEFT;
        _isFocusInput = true;
    }
}

void ItemDropUIRootManager::OnDpadRight(const Input::Controller&) 
{
    if (EnableInHierarchy)
    {
        _lastInputDir = InputDir::RIGHT;
        if (_isFocusArtifactNavi)
        {
            _isFocusInput = true;
        }   
    }
}

void ItemDropUIRootManager::OnDpadUp(const Input::Controller&) 
{
    if (EnableInHierarchy)
    {
        _lastInputDir = InputDir::UP;
        if (_isFocusArtifactNavi)
        {
            _isFocusInput = true;
        } 
    }
}

void ItemDropUIRootManager::OnDpadDown(const Input::Controller&) 
{
    if (EnableInHierarchy)
    {
        _lastInputDir = InputDir::DOWN;
        if (_isFocusArtifactNavi)
        {
            _isFocusInput = true;
        } 
    }
}

void ItemDropUIRootManager::OnLeftTumbStickDown(const Input::Controller& controller) 
{
    Input::Controller::StickBias bias = controller.GetLeftStickBias();
    switch (bias)
    {
    case Input::Controller::StickBias::BIAS_UP:
        OnDpadUp(controller);
        break;
    case Input::Controller::StickBias::BIAS_DOWN:
        OnDpadDown(controller);
        break;
    case Input::Controller::StickBias::BIAS_LEFT:
        OnDpadLeft(controller);
        break;
    case Input::Controller::StickBias::BIAS_RIGHT:
        OnDpadRight(controller);
        break;
    default:
        break;
    }
}

