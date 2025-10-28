#include "pchScripts.h"
#include "DescriptionPanel.h"
#include <Regex>

#include "UI/Elements/Text/TextElement.h"
#include "UI/Elements/Image/ImageElement.h"
#include "UI/Wrappers/Ratio/RatioWrapper.h"

UMREAL_COMPONENT(DescriptionPanel)

struct HexToColor
{
    Color operator()(const std::string& hex) const
    {
        static const std::regex HEX_COLOR_REGEX(R"(^#([A-Fa-f0-9]{6}|[A-Fa-f0-9]{8})$)");

        if (!std::regex_match(hex, HEX_COLOR_REGEX))
            return {0.0f, 0.0f, 0.0f, 1.0f}; // Invalid hex format

        std::string hexColor = hex;
        hexColor             = hexColor.substr(1);

        unsigned int      r = 0, g = 0, b = 0, a = 255;
        std::stringstream ss;
        ss << std::hex << hexColor.substr(0, 2);
        ss >> r;
        ss.clear();
        ss << std::hex << hexColor.substr(2, 2);
        ss >> g;
        ss.clear();
        ss << std::hex << hexColor.substr(4, 2);
        ss >> b;

        if (hexColor.length() == 8)
        {
            ss.clear();
            ss << std::hex << hexColor.substr(6, 2);
            ss >> a;
        }

        float red   = static_cast<float>(r) / 255.0f;
        float green = static_cast<float>(g) / 255.0f;
        float blue  = static_cast<float>(b) / 255.0f;
        float alpha = static_cast<float>(a) / 255.0f;

        return {red, green, blue, alpha};
    }
};

struct ParseData
{
    std::vector<ElementData> operator()(const std::string& content) const
    {
        std::vector<ElementData> elements;

        if (content.empty())
            return elements;

        pugi::xml_document doc;
        if (pugi::xml_parse_result result = doc.load_string(content.c_str()); !result)
        {
            UmLogger.Log(LogLevel::LEVEL_WARNING, std::format("XML parsing failed: {}", result.description()));
            return elements;
        }

        for (const pugi::xml_node description = doc.child("Description"); auto node : description.children())
        {
            if (std::strcmp(node.name(), "Text") == 0)
            {
                std::string textContent = node.child_value();
                Color       color       = HexToColor()(node.attribute("color").as_string("#000000"));

                TextAttributes attributes{.Content = textContent, .Color = color};
                ElementData    elementData{.Type = ElementType::TEXT, .Data = attributes};
                elements.push_back(elementData);
            }
            else if (std::strcmp(node.name(), "Image") == 0)
            {
                File::Guid guid;
                if (auto guidAttribute = node.attribute("guid"); !guidAttribute.empty())
                {
                    guid = File::Guid(node.attribute("guid").as_string());
                }
                else if (auto pathAttribute = node.attribute("path"); !pathAttribute.empty())
                {
                    File::Path path = File::Path(pathAttribute.as_string());
                    guid = path.ToGuid();
                }
                else if (auto assetAttribute = node.attribute("asset"); !assetAttribute.empty())
                {
                    int id = std::stoi(assetAttribute.as_string());
                    guid   = UmFileSystem.GetGuidFromAssetID(id);
                }
                else
                {
                    UmLogger.Log(LogLevel::LEVEL_WARNING, "Image element missing 'guid' or 'path' attribute.");
                    continue; // Skip this element if no valid guid or path is provided
                }
                ImageAttributes attributes{.Guid = guid};
                ElementData     elementData{.Type = ElementType::IMAGE, .Data = attributes};
                elements.push_back(elementData);
            }
        }

        return elements;
    }
};

DescriptionPanel::DescriptionPanel()
{
    FontPath.SetInputAutoEvent([this]() {
        if (ImGui::BeginDragDropTarget())
        {
            if (const ImGuiPayload* payLoad = ImGui::AcceptDragDropPayload(DragDropAsset::KEY))
            {
                const DragDropAsset::Data* data = static_cast<DragDropAsset::Data*>(payLoad->Data);
                if (const auto extension = data->GetPath().extension(); extension == L".png")
                {
                    _Guid            = data->GetGuid();
                    ReflectFields->Guid = _Guid.string();
                    UpdateContent();
                }
            }
            ImGui::EndDragDropTarget();
        }
    });

    Description.SetInputAutoEvent([this]() {
        if (ImGui::BeginDragDropTarget())
        {
            if (const ImGuiPayload* payLoad = ImGui::AcceptDragDropPayload(DragDropAsset::KEY))
            {
                const DragDropAsset::Data* data = static_cast<DragDropAsset::Data*>(payLoad->Data);
                if (const auto extension = data->GetPath().extension(); extension == L".png")
                {
                    const std::string newDescription = ReflectFields->Description + data->GetGuid().string();
                    ReflectFields->Description       = newDescription;
                    UpdateContent();
                }
            }
            ImGui::EndDragDropTarget();
        }
    });
}

void DescriptionPanel::SetOpacity(const float opacity)
{
    ReflectFields->Alpha = std::clamp(opacity, 0.0f, 1.0f);
    UpdateAlpha();
}

void DescriptionPanel::DeserializedReflectEvent()
{
    HorizontalPanel::DeserializedReflectEvent();

    const File::Guid guid = ReflectFields->Guid;
    if (const auto path = guid.ToPath(); !path.IsNull())
    {
        _Guid = path.ToGuid();
    }
}

void DescriptionPanel::ImGuiDrawPropertysEvent()
{
    HorizontalPanel::ImGuiDrawPropertysEvent();

    if (_isDebug)
    {
        const std::string& guid = ReflectFields->Guid;
        ImGuiDebug()("Font GUID", guid);
    }
}

void DescriptionPanel::Awake()
{
    HorizontalPanel::Awake();

    UpdateContent();
}

void DescriptionPanel::UpdateContent()
{
    if (const bool enableInHierarchy = EnableInHierarchy; enableInHierarchy)
    {
        EraseChild();
        MakeChild();
        InvalidateMeasure();
    }
}

void DescriptionPanel::EraseChild() const
{
    Transform& transform = this->transform;

    const int                childCount = transform.GetChildCount();
    std::vector<GameObject*> children;
    for (int i = 0; i < childCount; ++i)
    {
        const Transform* childTransform  = transform.GetChild(i);
        GameObject&      childGameObject = childTransform->gameObject;
        children.push_back(&childGameObject);
    }
    for (GameObject* child : children)
    {
        GameObject::Destroy(child);
    }
    children.clear();
}

void DescriptionPanel::MakeChild()
{
    Transform&        transform = this->transform;
    const std::string text      = ReflectFields->Description;

    for (const std::vector<ElementData> elementData = ParseData()(text); const auto& [Type, Data] : elementData)
    {
        const std::shared_ptr<GameObject> child =
            NewGameObject(GameObject::Helper::GenerateUniqueName("Description Child"));
        switch (Type)
        {
        case ElementType::TEXT: {
            TextElement& element  = child->AddComponent<TextElement>();
            auto [content, color] = std::get<TextAttributes>(Data);
            element.SetFont(_Guid);
            element.HorizontalFillMode = FillMode::WRAP;
            element.VerticalFillMode   = FillMode::WRAP;
            element.Text               = content;
            color.w                    = ReflectFields->Alpha;
            element.Color              = color;
            element.FontScale          = ReflectFields->FontScale;
            element.SetArtificial(true);
        }
        break;
        case ElementType::IMAGE: {
            RatioWrapper& ratio      = child->AddComponent<RatioWrapper>();
            ratio.HorizontalFillMode = FillMode::FILL;
            ratio.VerticalFillMode   = FillMode::FILL;
            const std::shared_ptr<GameObject> imageChild =
                NewGameObject(GameObject::Helper::GenerateUniqueName("Image Element"));
            auto [guid]           = std::get<ImageAttributes>(Data);
            ImageElement& element = imageChild->AddComponent<ImageElement>();
            element.SetImage(guid);
            element.HorizontalFillMode = FillMode::FILL;
            element.VerticalFillMode   = FillMode::FILL;
            element.Alpha              = ReflectFields->Alpha;
            element.SetArtificial(true);
            imageChild->transform->SetParent(child->transform, true);
        }
        break;
        }
        child->transform->SetParent(transform, true);
    }
}

void DescriptionPanel::UpdateAlpha()
{
    const float alpha = ReflectFields->Alpha;
    Transform&  myTransform = transform;
    Transform::ForeachBFS(transform, [alpha](Transform* t) 
    {
        GameObject& object = t->gameObject;
        if (TextElement* textElement = object.GetComponent<TextElement>(); nullptr != textElement)
        {
            textElement->SetOpacity(alpha);
        }
        else if (ImageElement* imageElement = object.GetComponent<ImageElement>(); nullptr != imageElement)
        {
            imageElement->SetOpacity(alpha);
        }
    });
}
