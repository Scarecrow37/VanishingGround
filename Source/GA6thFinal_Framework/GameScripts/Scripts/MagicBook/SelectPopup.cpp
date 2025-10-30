#include "pchScripts.h"
#include "SelectPopup.h"
#include "Input/InputOkCancelComponent/InputOkCancelComponent.h"
#include "SceneTransition/SceneTransitionComponent.h"
#include "UI/Elements/Image/ImageElement.h"
#include "UI/Elements/Text/TextElement.h"
#include "SelectMagicBook.h"

#include "PlayerSystem/PlayerSystem.h"
#include "Stats/Player/PlayerStats.h"
#include "Stats/Player/PlayerStatsComponent.h"
#include "RevelationSystem/RevelationSystem.h"
#include "WeaponSystem/WeaponSystem.h"
#include "WeaponSystem/WeaponTable/WeaponTableComponent.h"

UMREAL_COMPONENT(SelectPopup)

SelectPopup::SelectPopup()
{
    NextScene.SetInputAutoEvent([this]() {
        if (ImGui::BeginDragDropTarget())
        {
            if (const ImGuiPayload* payLoad = ImGui::AcceptDragDropPayload(DragDropAsset::KEY))
            {
                const DragDropAsset::Data* data = static_cast<DragDropAsset::Data*>(payLoad->Data);
                if (const auto extension = data->GetPath().extension(); extension == L".UmScene")
                {
                    ReflectFields->NextSceneGuid = data->GetGuid().string();
                }
            }
            ImGui::EndDragDropTarget();
        }
    });
}

SelectPopup::~SelectPopup() = default;

void SelectPopup::SetSelectMagicBook(const std::string& imagePath, const std::string& title, SelectMagicBook* magicBook)
{
    Transform::ForeachBFS(transform, [this, imagePath, title](Transform* tr) {
        auto&            gameObject = tr->gameObject;
        std::string_view objectName = gameObject->ToString();

        if ("Book" == objectName)
        {
            if (ImageElement* element = tr->gameObject->GetComponent<ImageElement>())
            {
                element->SetImage(UmFileSystem.GetGuidFromPath(imagePath));
            }
        }
        else if ("Select Magic Book" == objectName)
        {
            if (TextElement* element = tr->gameObject->GetComponent<TextElement>())
            {
                element->Text = title;
            }
        }
    });

    if (auto component = GetComponent<InputOkCancelComponent>())
    {
        component->GetOkOrCancel([this](bool isOk) 
        {
            if (isOk)
            {
                if (_lastSelectMagicBook)
                {
                    using namespace u8_literals;
                    const std::string& bookName = _lastSelectMagicBook->GetBookID();
                    if (bookName == u8"803005"_c_str)
                    {
                        if (PlayerSystem* playerSystem = SingletonComponent<PlayerSystem>::GetInstance())
                        {
                            if (auto component = playerSystem->GetPlayerStats())
                            {
                                auto& stats = component->GetStats();
                                stats.MaxHP = 70;
                                stats.StunResistance = 3;
                            }
                        }

                        if (WeaponSystem* weaponSystem = SingletonComponent<WeaponSystem>::GetInstance())
                        {
                            if (WeaponTableComponent* table = SingletonComponent<WeaponTableComponent>::GetInstance())
                            {
                                if (const WeaponElement* weapon = table->GetWeaponToID(u8"201007"))
                                {
                                    weaponSystem->EquipWeapon(0, *weapon);
                                }         
                                if (const WeaponElement* weapon = table->GetWeaponToID(u8"201000"))
                                {
                                    weaponSystem->EquipWeapon(1, *weapon);
                                }     
                                if (const WeaponElement* weapon = table->GetWeaponToID(u8"201100"))
                                {
                                    weaponSystem->EquipWeapon(2, *weapon);
                                }     
                                if (const WeaponElement* weapon = table->GetWeaponToID(u8"201200"))
                                {
                                    weaponSystem->EquipWeapon(3, *weapon);
                                }     
                            }                          
                        }

                        if (RevelationSystem* system = SingletonComponent<RevelationSystem>::GetInstance())
                        {
                            system->ClearPlayerElements();

                            if (RevelationElement* revelation = system->FindElementWithID(u8"202000"))
                            {
                                system->PushBackRevelation(*revelation);
                            }
                            if (RevelationElement* revelation = system->FindElementWithID(u8"202001"))
                            {
                                system->PushBackRevelation(*revelation);
                            }
                            if (RevelationElement* revelation = system->FindElementWithID(u8"202005"))
                            {
                                system->PushBackRevelation(*revelation);
                            }
                            if (RevelationElement* revelation = system->FindElementWithID(u8"202020"))
                            {
                                system->PushBackRevelation(*revelation);
                            }
                            if (RevelationElement* revelation = system->FindElementWithID(u8"202032"))
                            {
                                system->PushBackRevelation(*revelation);
                            }
                        }
                    }
                    else if (bookName == u8"803006"_c_str)
                    {
                        if (PlayerSystem* playerSystem = SingletonComponent<PlayerSystem>::GetInstance())
                        {
                            if (auto component = playerSystem->GetPlayerStats())
                            {
                                auto& stats          = component->GetStats();
                                stats.MaxHP          = 65;
                                stats.StunResistance = 3;
                            }
                        }

                        if (WeaponSystem* weaponSystem = SingletonComponent<WeaponSystem>::GetInstance())
                        {
                            if (WeaponTableComponent* table = SingletonComponent<WeaponTableComponent>::GetInstance())
                            {
                                if (const WeaponElement* weapon = table->GetWeaponToID(u8"201000"))
                                {
                                    weaponSystem->EquipWeapon(0, *weapon);
                                }
                                if (const WeaponElement* weapon = table->GetWeaponToID(u8"201105"))
                                {
                                    weaponSystem->EquipWeapon(1, *weapon);
                                }
                                if (const WeaponElement* weapon = table->GetWeaponToID(u8"201100"))
                                {
                                    weaponSystem->EquipWeapon(2, *weapon);
                                }
                                if (const WeaponElement* weapon = table->GetWeaponToID(u8"201200"))
                                {
                                    weaponSystem->EquipWeapon(3, *weapon);
                                }
                            }
                        }

                        if (RevelationSystem* system = SingletonComponent<RevelationSystem>::GetInstance())
                        {
                            system->ClearPlayerElements();

                            if (RevelationElement* revelation = system->FindElementWithID(u8"202005"))
                            {
                                system->PushBackRevelation(*revelation);
                            }
                            if (RevelationElement* revelation = system->FindElementWithID(u8"202003"))
                            {
                                system->PushBackRevelation(*revelation);
                            }
                            if (RevelationElement* revelation = system->FindElementWithID(u8"202015"))
                            {
                                system->PushBackRevelation(*revelation);
                            }
                            if (RevelationElement* revelation = system->FindElementWithID(u8"202029"))
                            {
                                system->PushBackRevelation(*revelation);
                            }
                            if (RevelationElement* revelation = system->FindElementWithID(u8"300001"))
                            {
                                system->PushBackRevelation(*revelation);
                            }
                        }
                    }
                    else if (bookName == u8"803007"_c_str)
                    {
                        if (PlayerSystem* playerSystem = SingletonComponent<PlayerSystem>::GetInstance())
                        {
                            if (auto component = playerSystem->GetPlayerStats())
                            {
                                auto& stats          = component->GetStats();
                                stats.MaxHP          = 80;
                                stats.StunResistance = 4;
                            }
                        }

                        if (WeaponSystem* weaponSystem = SingletonComponent<WeaponSystem>::GetInstance())
                        {
                            if (WeaponTableComponent* table = SingletonComponent<WeaponTableComponent>::GetInstance())
                            {
                                if (const WeaponElement* weapon = table->GetWeaponToID(u8"201000"))
                                {
                                    weaponSystem->EquipWeapon(0, *weapon);
                                }
                                if (const WeaponElement* weapon = table->GetWeaponToID(u8"201100"))
                                {
                                    weaponSystem->EquipWeapon(1, *weapon);
                                }
                                if (const WeaponElement* weapon = table->GetWeaponToID(u8"201206"))
                                {
                                    weaponSystem->EquipWeapon(2, *weapon);
                                }
                                if (const WeaponElement* weapon = table->GetWeaponToID(u8"201200"))
                                {
                                    weaponSystem->EquipWeapon(3, *weapon);
                                }
                            }
                        }

                        if (RevelationSystem* system = SingletonComponent<RevelationSystem>::GetInstance())
                        {
                            system->ClearPlayerElements();

                            if (RevelationElement* revelation = system->FindElementWithID(u8"202000"))
                            {
                                system->PushBackRevelation(*revelation);
                            }
                            if (RevelationElement* revelation = system->FindElementWithID(u8"202008"))
                            {
                                system->PushBackRevelation(*revelation);
                            }
                            if (RevelationElement* revelation = system->FindElementWithID(u8"202021"))
                            {
                                system->PushBackRevelation(*revelation);
                            }
                            if (RevelationElement* revelation = system->FindElementWithID(u8"202024"))
                            {
                                system->PushBackRevelation(*revelation);
                            }
                            if (RevelationElement* revelation = system->FindElementWithID(u8"202030"))
                            {
                                system->PushBackRevelation(*revelation);
                            }
                        }
                    }
                }
           
                File::Path path = File::Guid(ReflectFields->NextSceneGuid).ToPath();
                if (auto transitionManager = SingletonComponent<SceneTransitionComponent>::GetInstance())
                {
                    transitionManager->SceneTransitionFade("in", "out",
                                                           [path]() { UmSceneManager.LoadScene(path.string()); });
                }
                else
                {
                    UmSceneManager.LoadScene(path.string());
                }
            }
            else
            {
                gameObject->SetActive(false);

                if (_lastSelectMagicBook)
                {
                    _lastSelectMagicBook->Focus();
                }
            }
        });
    }

    _lastSelectMagicBook = magicBook;
}