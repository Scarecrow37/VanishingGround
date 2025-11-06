#include "pchScripts.h"
#include "SkeletalMeshRenderer.h"
#include "Animation/AnimationComponent.h"
#include "GraphicsEngine/Interface/IMeshRenderer.h"
#include "GraphicsEngine/Interface/IAnimator.h"

UMREAL_COMPONENT(SkeletalMeshRenderer)

SkeletalMeshRenderer::SkeletalMeshRenderer() 
{
    FilePath.SetInputAutoEvent([this]() 
    {
        if (ImGui::BeginDragDropTarget())
        {
            if (const ImGuiPayload* payLoad = ImGui::AcceptDragDropPayload(DragDropAsset::KEY))
            {
                DragDropAsset::Data* data = static_cast<DragDropAsset::Data*>(payLoad->Data);
                File::Path path = data->GetPath();
                const auto extension = path.extension();
                if (extension == L".fbx" || extension == L".UmModel")
                {
                    _Guid = data->GetGuid();
                    ReflectFields->Basefields.get().Guid = _Guid.string();
                    UmSceneManager.ResourceManager.RequestModelResource(this, _Guid, [this]() { LoadModel(); });
                }
            }
            ImGui::EndDragDropTarget();
        }
    });
}

SkeletalMeshRenderer::~SkeletalMeshRenderer() = default;

const Matrix* SkeletalMeshRenderer::GetBoneMatrixFromMappingKey(const std::string& key)
{
    if (_boneMatrixMappingTable.contains(key))
    {
        return _boneMatrixMappingTable[key];
    }
    return nullptr;
}

void SkeletalMeshRenderer::Reset()
{
    MakeMeshRenderer(transform->GetWorldMatrix());
    
    if (false == _Guid.IsNull())
    {
        UmSceneManager.ResourceManager.RequestModelResource(this, _Guid, [this]() { LoadModel(); });
    } 
}

void SkeletalMeshRenderer::DeserializedReflectEvent() 
{
    File::Guid guid = ReflectFields->Basefields.get().Guid;
    _Guid        = guid;
}

void SkeletalMeshRenderer::ImGuiDrawPropertysEvent() 
{
    Base::ImGuiDrawPropertysEvent();

    if (nullptr != Renderer)
    {
        if (nullptr == Renderer->GetModel())
        {
            ImGui::Separator();
            ImGui::Text("NULL Model");
        }
        else
        {
            std::function<bool(std::string&)> comboList = [this](std::string& out) {
                bool isDirty = false;
                if (Renderer)
                {
                    if (auto model = Renderer->GetModel())
                    {
                        const auto& boneNames = model->GetBoneNameList();
                        if (ImGuiHelper::BeginComboInput("##AnimName", &out))
                        {
                            for (int i = 0; i < boneNames.size(); ++i)
                            {
                                if (ImGui::Selectable(boneNames[i].c_str()))
                                {
                                    out     = boneNames[i];
                                    isDirty = true;
                                }
                            }
                            ImGui::EndCombo();
                        }
                    }
                }
                return isDirty;
            };

            static std::string                        boneKey;
            static std::vector<std::function<void()>> delayEvent;
            if (ImGui::BeginTable("BoneMapping##Details", 2, ImGuiTableFlags_BordersInnerH | ImGuiTableFlags_RowBg))
            {
                ImGui::TableSetupColumn("Key", ImGuiTableColumnFlags_WidthStretch, 0.3f);
                ImGui::TableSetupColumn("Animation Name", ImGuiTableColumnFlags_WidthStretch, 0.7f);
                ImGui::TableHeadersRow();

                int seed = 0;
                for (auto& [key, bone] : ReflectFields->BoneKeyMap)
                {
                    ImGui::PushID(seed);

                    ImGui::TableNextRow();
                    ImGui::TableSetColumnIndex(0);
                    {
                        ImGui::Selectable(key.c_str());
                        ImGuiHelper::HoveredToolTip(key.c_str());
                    }

                    ImGui::TableSetColumnIndex(1);
                    {
                        if (comboList(boneKey))
                        {
                            bone = boneKey;
                        }
                        float height = ImGui::GetItemRectSize().y;
                        ImGuiHelper::HoveredToolTip(bone.c_str());
                        ImGui::SameLine();
                        if (ImGui::Button("-", ImVec2(height, height)))
                        {
                            delayEvent.push_back([this, key]() { ReflectFields->BoneKeyMap.erase(key); });
                        }
                    }
                    ImGui::PopID();
                    ++seed;
                }
                ImGui::PushID(seed);
                ImGui::TableNextRow();
                ImGui::TableSetColumnIndex(0);
                static std::string newKey;
                static std::string boneName;
                ImVec2             availSize = ImGui::GetContentRegionAvail();
                ImGui::SetNextItemWidth(availSize.x);
                ImGui::InputTextWithHint("##NewKey", "New Key...", &newKey);

                ImGui::TableSetColumnIndex(1);
                comboList(boneName);
                float height = ImGui::GetItemRectSize().y;
                ImGuiHelper::HoveredToolTip(boneName.c_str());
                ImGui::SameLine();
                if (ImGui::Button("+", ImVec2(height, height)))
                {
                    delayEvent.push_back([this]() {
                        ReflectFields->BoneKeyMap[newKey] = boneName;
                        newKey.clear();
                        boneName.clear();
                    });
                }
                ImGui::PopID();
                ImGui::EndTable();
            }

            for (auto& event : delayEvent)
            {
                if (event)
                {
                    event();
                }
            }
        }
    }
}

void SkeletalMeshRenderer::LoadModel()
{
    if (Renderer)
    {
        std::string path = FilePath;
        if (path != File::NULL_PATH)
        {
            std::wstring modelPath = U8ToWString(path);
            UmGraphics.LoadResource(modelPath, Renderer.Get());

            if (IAnimator* animator = Renderer->GetAnimator())
            {                                
                animator->SetActive(&EnableInHierarchy);
                UmGraphics.RegisterComponent(animator);
                Renderer->OnCustomDepth(PostProcess::BLOOM | PostProcess::IS_SKELETAL_MESH);
                this->InitMaterial();
            }

            OnChangedModel();
        }
    }
}

void SkeletalMeshRenderer::OnChangedModel() 
{
    if (Renderer)
    {
        IAnimator* animator = Renderer->GetAnimator();
        if (animator)
        {
            if (AnimationComponent* animationComponent = GetComponent<AnimationComponent>())
            {
                animationComponent->SetAnimator(animator);
                animationComponent->ChangeDefaultAnimation();
            }
            for (auto& [key, bone] : ReflectFields->BoneKeyMap)
            {
                _boneMatrixMappingTable[key] = animator->FindBoneMatrix(bone.c_str());
            }
        }
    }
}
