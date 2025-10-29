#include "pchScripts.h"
#include "SelectMagicBook.h"
#include "UI/Elements/Image/ImageElement.h"
#include "UI/Elements/Text/TextElement.h"
#include "ExcelDataSystem/ExcelDataSystem.h"
#include "UI/Panels/Description/DescriptionPanel.h"

UMREAL_COMPONENT(SelectMagicBook)

SelectMagicBook::SelectMagicBook()
{
    FocusImagePath.SetInputAutoEvent([this]() {
        if (ImGui::BeginDragDropTarget())
        {
            if (const ImGuiPayload* payLoad = ImGui::AcceptDragDropPayload(DragDropAsset::KEY))
            {
                DragDropAsset::Data* data      = static_cast<DragDropAsset::Data*>(payLoad->Data);
                File::Path           path      = data->GetPath();
                const auto           extension = path.extension();

                if (extension == L".png")
                {
                    ReflectFields->FocusImagePath = path.string();
                }
            }
            ImGui::EndDragDropTarget();
        }
    });

    UnFocusImagePath.SetInputAutoEvent([this]() {
        if (ImGui::BeginDragDropTarget())
        {
            if (const ImGuiPayload* payLoad = ImGui::AcceptDragDropPayload(DragDropAsset::KEY))
            {
                DragDropAsset::Data* data      = static_cast<DragDropAsset::Data*>(payLoad->Data);
                File::Path           path      = data->GetPath();
                const auto           extension = path.extension();

                if (extension == L".png")
                {
                    ReflectFields->UnFocusImagePath = path.string();
                }
            }
            ImGui::EndDragDropTarget();
        }
    });
}

SelectMagicBook::~SelectMagicBook() = default;

void SelectMagicBook::Start()
{
    FocusOut(FocusCallType::INITIAL);
}

void SelectMagicBook::FocusIn(FocusCallType callType)
{
    Base::FocusIn(callType);

    std::unique_ptr<ExcelDataBase> dataBase;
    if (ExcelDataSystem* system = SingletonComponent<ExcelDataSystem>::GetInstance())
    {
        dataBase = system->FindExcelDataBase(u8"텍스트");
    }

    Transform::ForeachBFS(transform, [this, &dataBase](Transform* tr) {
        auto&            gameObject = tr->gameObject;
        std::string_view objectName = gameObject->ToString();

        if ("Book" == objectName)
        {
            ChangeBookImage(gameObject, ReflectFields->FocusImagePath);
        }
        else if ("Title" == objectName)
        {
            ChangeTitle(gameObject, 1.f);
        }
        else if ("Description" == objectName)
        {            
            if (dataBase)
            {
                ChangeDescription(gameObject, dataBase.get(), ReflectFields->FocusDescriptionID);
            }
        }
        else if ("HP HUD" == objectName)
        {
            ChangeHPHUD(gameObject, 1.f);
        }
    });
}

void SelectMagicBook::FocusOut(FocusCallType callType)
{
    Base::FocusOut(callType);

    std::unique_ptr<ExcelDataBase> dataBase;
    if (ExcelDataSystem* system = SingletonComponent<ExcelDataSystem>::GetInstance())
    {
        dataBase = system->FindExcelDataBase(u8"텍스트");
    }

    Transform::ForeachBFS(transform, [this, &dataBase](Transform* tr) {
        auto&            gameObject = tr->gameObject;
        std::string_view objectName = gameObject->ToString();

        if ("Book" == objectName)
        {
            ChangeBookImage(gameObject, ReflectFields->UnFocusImagePath);
        }
        else if ("Title" == objectName)
        {
            ChangeTitle(gameObject, 0.5f);
        }
        else if ("Description" == objectName)
        {
            if (dataBase)
            {
                ChangeDescription(gameObject, dataBase.get(), ReflectFields->UnFocusDescriptionID);
            }
        }
        else if ("HP HUD" == objectName)
        {
            ChangeHPHUD(gameObject, 0.5f);
        }
    });
}

void SelectMagicBook::Submit()
{
    Base::Submit();

    if (auto object = GameObject::Find("Select Popup").lock())
    {
        object->SetActive(true);
    } 
}

void SelectMagicBook::ImGuiDrawPropertysEvent()
{
    Base::ImGuiDrawPropertysEvent();

    if (ImGui::BeginTable("SelectMagicBook", 2, ImGuiTableFlags_Borders | ImGuiTableFlags_RowBg))
    {
        ImGui::TableNextRow();
        ImGui::TableSetColumnIndex(0);
        ImGuiHelper::CenterText("FocusDescriptionID");
        ImGui::TableSetColumnIndex(1);
        ImGui::PushItemWidth(-FLT_MIN);
        ImGui::InputText("##FocusID", &ReflectFields->FocusDescriptionID);
        ImGui::PopItemWidth();

        ImGui::TableNextRow();
        ImGui::TableSetColumnIndex(0);
        ImGuiHelper::CenterText("UnFocusDescriptionID");
        ImGui::TableSetColumnIndex(1);
        ImGui::PushItemWidth(-FLT_MIN);
        ImGui::InputText("##UnFocusID", &ReflectFields->UnFocusDescriptionID);
        ImGui::PopItemWidth();

        ImGui::EndTable();
    }
}

void SelectMagicBook::ChangeBookImage(GameObject& targetGameObject, const std::string& imagePath)
{
    targetGameObject.GetComponent<ImageElement>()->SetImage(UmFileSystem.GetGuidFromPath(imagePath));
}

void SelectMagicBook::ChangeTitle(GameObject& targetGameObject, const float alpha)
{
    TextElement* textElement = targetGameObject.GetComponent<TextElement>();
    Color        color       = textElement->Color;
    color.w                  = alpha;
    textElement->Color       = color;
}

void SelectMagicBook::ChangeDescription(GameObject& targetGameObject, ExcelDataBase* dataBase, const std::string& ID)
{
    size_t rowIndex = dataBase->FindRowIndex(reinterpret_cast<const char8_t*>(ID.c_str()), u8"ID");
    if (rowIndex != dataBase->FIND_INDEX_FAIL)
    {
        std::string_view description = dataBase->FindData(rowIndex, u8"Content");
        if (description != dataBase->FIND_STR_FAIL)
        {
            targetGameObject.GetComponent<DescriptionPanel>()->Description = description.data();
        }
    }
}

void SelectMagicBook::ChangeHPHUD(GameObject& targetGameObject, const float alpha)
{
    Transform::ForeachBFS(targetGameObject.transform, [this, alpha](Transform* tr) {
        auto&            gameObject = tr->gameObject;
        std::string_view objectName = gameObject->ToString();

        if ("Frame" == objectName)
        {
            gameObject->GetComponent<ImageElement>()->Alpha = alpha;
        }
        else if ("Gage" == objectName)
        {
            gameObject->GetComponent<ImageElement>()->Alpha = alpha;
        }
        else if ("HP" == objectName)
        {
            TextElement* textElement = gameObject->GetComponent<TextElement>();
            Color        color       = textElement->Color;
            color.w                  = alpha;
            textElement->Color       = color;
        }
    });
}