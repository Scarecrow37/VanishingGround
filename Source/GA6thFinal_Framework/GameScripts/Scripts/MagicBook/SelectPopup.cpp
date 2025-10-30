#include "pchScripts.h"
#include "SelectPopup.h"
#include "Input/InputOkCancelComponent/InputOkCancelComponent.h"
#include "SceneTransition/SceneTransitionComponent.h"
#include "UI/Elements/Image/ImageElement.h"
#include "UI/Elements/Text/TextElement.h"
#include "SelectMagicBook.h"

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
        component->GetOkOrCancel([this](bool isOk) {
            if (isOk)
            {
                // TODO:: 씬 전환 효과 및 선택된 마도서로 게임 시작
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