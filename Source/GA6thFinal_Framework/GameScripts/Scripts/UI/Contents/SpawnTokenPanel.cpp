#include "pchScripts.h"
#include "SpawnTokenPanel.h"
#include "TokenElement.h"
#include "Token/TokenSystem.h"
#include "Utility/SingletonHelper.h"

class TokenSystem;
UMREAL_COMPONENT(SpawnTokenPanel)

SpawnTokenPanel::SpawnTokenPanel() = default;

std::weak_ptr<TokenElement> SpawnTokenPanel::MakeToken(const int tokenID)
{
    std::weak_ptr<TokenElement> weakTokenElement;

    if (TokenSystem* tokenSystem = SingletonComponent<TokenSystem>::GetInstance())
    {
        if (const TokenData* tokenData = tokenSystem->GetTokenDataFromID(tokenID))
        {
            weakTokenElement = GetTokenElement();
            if (const std::shared_ptr<TokenElement> tokenElement = weakTokenElement.lock())
            {
                tokenElement->gameObject->SetActive(true);

                const SIZE                    size       = Size;
                const POINT                   beginPoint = POINT{.x = 0, .y = size.cy};
                constexpr POINT               endPoint   = POINT{.x = 0, .y = 0};
                const TokenElement::SetupData data{
                    .BeginPoint   = beginPoint,
                    .EndPoint     = endPoint,
                    .BeginOpacity = BeginOpacity,
                    .EndOpacity   = EndOpacity,
                    .Duration     = LifeTime,
                    .TokenName    = tokenData->Name,
                    .NameColor    = GetTokenColor(tokenID),
                    .IconGuid     = UmFileSystem.GetGuidFromAssetID(tokenData->ImageID),
                };

                tokenElement->Setup(data);
            }
        }
    }

    return weakTokenElement;
}

struct DragDropEvent
{
    bool operator()(File::Guid& out) const
    {
        bool result = false;
        if (ImGui::BeginDragDropTarget())
        {
            if (const ImGuiPayload* payLoad = ImGui::AcceptDragDropPayload(DragDropAsset::KEY))
            {
                const DragDropAsset::Data* data = static_cast<DragDropAsset::Data*>(payLoad->Data);
                const File::Path&          path = data->GetPath();
                if (const File::Path extension = path.extension(); extension == L".UmPrefab")
                {
                    result = true;
                    out    = path.ToGuid();
                }
                else
                {
                    UmLogger.Log(LogLevel::LEVEL_WARNING,
                                 reinterpret_cast<const char*>(u8"프리팹은 .UmPrefab 파일만 지정할 수 있습니다."));
                }
            }
            ImGui::EndDragDropTarget();
        }
        return result;
    }
};

void SpawnTokenPanel::Reset()
{
    UIComponent::Reset();

    TokenElementPrefab.SetInputAutoEvent([this]() {
        if (File::Guid dragDropGuid; DragDropEvent()(dragDropGuid))
        {
            ReflectFields->TokenElementPrefabGuid = dragDropGuid.string();
        }
    });

    _elapsedTime = CycleTime;

    EraseChild();
}

void SpawnTokenPanel::EraseChild() const
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

Color SpawnTokenPanel::GetTokenColor(const int tokenID) const
{
    Color                color{};
    const TokenColorMap& tokenColors = ReflectFields->TokenColors;
    ArrayColor           arrayColor;

    try
    {
        arrayColor = tokenColors.at(tokenID);
    }
    catch (...)
    {
        arrayColor = DEFAULT_COLOR;
        UmLogger.Log(LogLevel::LEVEL_INFO, u8"토큰에 부여된 색이 없습니다. 기본색으로 대체합니다.");
    }

    std::ranges::copy(arrayColor, &color.x);

    return color;
}

std::weak_ptr<TokenElement> SpawnTokenPanel::GetTokenElement()
{
    const int childCount = transform->GetChildCount();
    for (int i = 0; i < childCount; ++i)
    {
        if (Transform* childTransform = transform->GetChild(i))
        {
            GameObject& childGameObject = childTransform->gameObject;
            if (const TokenElement* tokenElement = childGameObject.GetComponentDynamic<TokenElement>())
            {
                if (false == tokenElement->EnableInHierarchy)
                {
                    return tokenElement->GetWeakPtrAs<TokenElement>();
                }
            }
        }
    }
    return CreateTokenElement();
}

std::weak_ptr<TokenElement> SpawnTokenPanel::CreateTokenElement()
{
    std::weak_ptr<TokenElement> weakTokenElement;
    const File::Guid prefab(ReflectFields->TokenElementPrefabGuid);

    if (const std::shared_ptr<GameObject> child = UmGameObjectFactory.DeserializeToGuid(prefab))
    {
        if (const TokenElement* tokenElement = child->GetComponentDynamic<TokenElement>())
        {
            child->transform->SetParent(transform);
            weakTokenElement = tokenElement->GetWeakPtrAs<TokenElement>();
        }
    }

    return weakTokenElement;
}

void SpawnTokenPanel::SpawnToken()
{
    _elapsedTime += UmTime.DeltaTime();
    const float cycleTime = ReflectFields->CycleTime;

    if (constexpr Mathf::CompareFloat compareFloat;
        compareFloat(_elapsedTime, cycleTime) == std::partial_ordering::greater)
    {
        _elapsedTime -= cycleTime;

        MakeToken(_tokenQueue.front());
        _tokenQueue.pop();
    }
}

void SpawnTokenPanel::EnqueueToken(const int tokenID)
{
    _tokenQueue.push(tokenID);
}

SIZE SpawnTokenPanel::MeasureOverride(const SIZE availableSize)
{
    const FillMode horizontalFillMode = HorizontalFillMode;
    const FillMode verticalFillMode   = VerticalFillMode;
    SIZE           desiredSize        = MinSize()(availableSize, _requestedSize, horizontalFillMode == FillMode::FILL,
                                 verticalFillMode == FillMode::FILL);

    const PADDING                   padding            = Padding;
    const SIZE                      childAvailableSize = desiredSize - padding.Size();
    SIZE                            childrenDesiredSize{};
    const std::vector<UIComponent*> children = Children;
    std::ranges::for_each(children, [childAvailableSize, &childrenDesiredSize](UIComponent* child) {
        child->Measure(childAvailableSize);
        const SIZE childDesiredSize = child->DesiredSize;
        childrenDesiredSize         = MaxSize()(childrenDesiredSize, childDesiredSize);
    });

    if (horizontalFillMode == FillMode::WRAP)
        desiredSize.cx = childrenDesiredSize.cx + padding.Horizontal();
    if (verticalFillMode == FillMode::WRAP)
        desiredSize.cy = childrenDesiredSize.cy + padding.Vertical();

    return desiredSize;
}

SIZE SpawnTokenPanel::ArrangeOverride(const SIZE finalSize)
{
    const SIZE                desiredSize           = DesiredSize;
    const SIZE                actualSize            = MinSize()(finalSize, desiredSize);
    const PADDING             padding               = Padding;
    const SIZE                childAvailableSize    = actualSize - padding.Size();
    const HorizontalAlignment horizontalAlign       = HorizontalAlign;
    const VerticalAlignment   verticalAlign         = VerticalAlign;
    const POINT               absoluteChildPosition = AbsolutePosition;

    const std::vector<UIComponent*> children = Children;
    std::ranges::for_each(children, [childAvailableSize, horizontalAlign, verticalAlign,
                                     absoluteChildPosition](UIComponent* child) {
        const SIZE  childDesiredSize = child->DesiredSize;
        const POINT alignPosition = AlignPoint()(horizontalAlign, verticalAlign, childAvailableSize - childDesiredSize);
        const POINT childPoint    = absoluteChildPosition + alignPosition;
        child->Arrange(childPoint, childAvailableSize);
    });

    return actualSize;
}

void SpawnTokenPanel::Update()
{
    UIComponent::Update();

    if (false == _tokenQueue.empty())
    {
        SpawnToken();
    }
}

namespace
{
    struct Row
    {
        void operator()(const int tokenID, std::array<float, 4>& color, const std::function<void()>& deleteCallback) const
        {
            // ID
            ImGui::TableSetColumnIndex(0);
            ImGui::Text("%d", tokenID);

            // Color
            ImGui::TableSetColumnIndex(1);
            float editColor[4];
            std::ranges::copy(color, editColor);
            if (ImGui::ColorEdit4("##Color", editColor))
            {
                std::ranges::copy(editColor, color.begin());
            }

            // Delete button
            const float height = ImGui::GetItemRectSize().y;
            ImGui::SameLine();
            if (ImGui::Button("-", ImVec2(height, height)))
            {
                deleteCallback();
            }
        }

        void operator()(int* newID, std::array<float, 4>* newColor, const bool isExisted,
                        const std::function<void()>& addCallback) const
        {
            if (newID == nullptr || newColor == nullptr)
            {
                return;
            }

            // ID
            ImGui::TableSetColumnIndex(0);
            ImVec2 availSize = ImGui::GetContentRegionAvail();
            ImGui::SetNextItemWidth(availSize.x);
            ImGuiHelper::StyleBuilder style;
            if (isExisted)
                style.PushStyleColor(ImGuiCol_Text, ImVec4(0.95f, 0.15f, 0.25f, 1.0f));
            else
                style.PushStyleColor(ImGuiCol_Text, ImVec4(0.15f, 0.75f, 0.35f, 1.0f));
            ImGui::InputInt("##NewKey", newID, 0);
            style.PopStyle();

            // Color
            ImGui::TableSetColumnIndex(1);
            availSize = ImGui::GetContentRegionAvail();
            ImGui::SetNextItemWidth(availSize.x - 60.0f);
            constexpr int flags = ImGuiInputTextFlags_ReadOnly | ImGuiInputTextFlags_EnterReturnsTrue;
            float         editColor[4];
            std::ranges::copy(*newColor, editColor);
            if (ImGui::ColorEdit4("##Color", editColor))
            {
                std::ranges::copy(editColor, newColor->begin());
            }

            // Add Button
            const float height = ImGui::GetItemRectSize().y;
            ImGui::SameLine();
            if (!isExisted && ImGui::Button("+", ImVec2(height, height)))
            {
                addCallback();
            }
        }
    };

    struct EraseLater
    {
        using Map = std::unordered_map<int, std::array<float, 4>>;
        explicit EraseLater(Map* map) : MapPointer(map) {}
        EraseLater(const EraseLater&)                = delete;
        EraseLater& operator=(const EraseLater&)     = delete;
        EraseLater(EraseLater&&) noexcept            = delete;
        EraseLater& operator=(EraseLater&&) noexcept = delete;
        ~EraseLater()
        {
            for (const auto& key : KeysToErase)
            {
                MapPointer->erase(key);
            }
            KeysToErase.clear();
        }

        void operator()(const int key) { KeysToErase.push_back(key); }

        Map*             MapPointer;
        std::vector<int> KeysToErase;
    };
} // namespace

void SpawnTokenPanel::ImGuiDrawPropertysEvent()
{
    UIComponent::ImGuiDrawPropertysEvent();

    static int tokenID = 0;
    ImGui::InputInt("Token ID", &tokenID, 0);
    ImGui::SameLine();
    if (ImGui::Button("Token!!!"))
    {
        EnqueueToken(tokenID);
    }

    auto& tokenColors = ReflectFields->TokenColors;
    if (ImGui::TreeNodeEx("Token Colors##details"))
    {
        // AddMapping Function
        if (ImGui::BeginTable("ColorTable##Details", 2, ImGuiTableFlags_BordersInnerH | ImGuiTableFlags_RowBg))
        {
            // Headers
            ImGui::TableSetupColumn("Token ID", ImGuiTableColumnFlags_WidthStretch, 0.3f);
            ImGui::TableSetupColumn("Color", ImGuiTableColumnFlags_WidthStretch, 0.6f);
            ImGui::TableHeadersRow();

            // Existing Colors
            EraseLater eraseLater(&tokenColors);
            for (auto& [tokenID, colorArray] : tokenColors)
            {
                ImGui::PushID(tokenID);
                ImGui::TableNextRow();
                Row()(tokenID, colorArray, [tokenID, &eraseLater]() { eraseLater(tokenID); });
                ImGui::PopID();
            }

            // New Color;
            ImGui::TableNextRow();
            Row()(&_newID, &_newColor, tokenColors.contains(_newID), [this, &tokenColors]() {
                if (auto [iterator, isSucceed] = tokenColors.try_emplace(_newID, _newColor); isSucceed)
                {
                    _newID = 0;
                    _newColor = DEFAULT_COLOR;
                }
            });
            ImGui::EndTable();
        }

        ImGui::TreePop();
    }
}

