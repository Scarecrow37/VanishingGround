#include "pchScripts.h"
#include "MeshComponent.h"

MeshComponent::MeshComponent() 
    : 
    Component(Component::TYPE::MESH),
    Renderer(_pMeshRenderer)
{
  
}
MeshComponent::~MeshComponent()
{
    if (Renderer)
    {
        Renderer->SetDestroy();
        const std::shared_ptr<Animator>& animator = Renderer->GetAnimator();
        if (animator)
        {
            animator->SetDestroy();
        }     
    }
}

bool MeshComponent::HasModel() const
{
    return nullptr != Renderer->GetModel();
}

bool MeshComponent::HasAnimator() const
{
    return nullptr != Renderer->GetAnimator();
}

void MeshComponent::MakeMeshRenderer(MeshType renderType, const Vector3& position, const Vector3& scale, const Quaternion& rotation, const Matrix& world)
{
    if (nullptr == _pMeshRenderer)
    {
        _pMeshRenderer.reset(new MeshRenderer(renderType, position, scale, rotation, world, transform->HasChangedRef()));
        _pMeshRenderer->SetActive(&EnableInHierarchy);
        _pMeshRenderer->OnCustomDepth(PostProcess::BLOOM);
        UmGraphics.RegisterComponent("Game", _pMeshRenderer.get());

        if constexpr (IS_EDITOR)
        {
            UmGraphics.RegisterComponent("Editor", _pMeshRenderer.get());
        }
    } 
    else
    {
        //assert(!"이미 MeshRenderer가 존재합니다.");
    }
}

void MeshComponent::ImGuiDrawPropertysEvent()
{
    ImGui::Separator();

    if (ImGui::TreeNodeEx("Materials##MeshComponent"))
    {
        static UINT*         lastCustomDepth = 0;
        static MeshRenderer* lastRenderer = nullptr;
        static UINT          lastSelected = 0;

        const auto& model        = Renderer->GetModel();
        const auto& customDepths = Renderer->GetCustomDepths();
        const auto& meshes       = model->GetMeshes();
        auto&       materials    = model->GetMaterials();
        UINT        count        = (UINT)model->GetMeshCount();

        for (UINT i = 0; i < count; i++)
        {
            ImGui::PushID(i);
            bool isOpened = ImGui::TreeNodeEx(meshes[i]->GetName().data());
            if (ImGui::IsItemClicked())
            {
                if (lastRenderer)
                {
                    (*lastCustomDepth) &= ~PostProcess::OUTLINE;
                    lastRenderer->OffCustomDepth(PostProcess::OUTLINE, lastSelected);
                }

                ReflectFields->CustomDepth[i] |= PostProcess::OUTLINE;
                Renderer->OnCustomDepth(PostProcess::OUTLINE, i);
                
                // 마지막 선택한 값 기억
                lastSelected = i;
                lastRenderer = Renderer.get();
                lastCustomDepth = &ReflectFields->CustomDepth[i];
            }
            if (isOpened)
            {
                ImGuiTableFlags flags = ImGuiTableFlags_Borders | ImGuiTableFlags_RowBg | ImGuiTableFlags_Sortable;
                if (ImGui::BeginTable("##MeshComponent", 2, ImGuiTableFlags_Borders))
                {
                    //ImGui::TableNextRow();
                    //{
                    //    ImGui::TableNextColumn();
                    //    {
                    //        ImGui::Text("Shading Model");
                    //    }
                    //    ImGui::TableNextColumn();
                    //    {
                    //        static const char* names[] = {"Unlit", "Default Lit"};
                    //        if (ImGui::Combo("##ShadingModel", (int*)&_materials[i].Mode, names, 2))
                    //        {
                    //            //_materials[i].CullMode;
                    //        }
                    //    }
                    //    ImGui::TableNextColumn();
                    //}
                    ImGui::TableNextRow();
                    {
                        ImGui::TableNextColumn();
                        {
                            ImGui::Text("Is Two Sided(Shared)");
                        }
                        ImGui::TableNextColumn();
                        {
                            ImGui::Checkbox("##IsTwoSided", &materials[i].IsTwoSided);
                            ReflectFields->IsTwoSided[i] = materials[i].IsTwoSided;
                        }
                        ImGui::TableNextColumn();
                    }

                    ImGui::TableNextRow();
                    {
                        ImGui::TableNextColumn();
                        {
                            ImGui::Text("CustomDepth");
                        }
                        ImGui::TableNextColumn();
                        {
                            if (ImGui::BeginCombo("##CustomDepth", "Drop Down List"))
                            {
                                bool isBloom = customDepths[i] & PostProcess::BLOOM ? true : false;
                                ImGuiHelper::TextWithVerticalSeparator("Bloom");
                                if (ImGui::Checkbox("##Bloom", &isBloom))
                                {
                                    isBloom ? Renderer->OnCustomDepth(PostProcess::BLOOM, i) : 
                                              Renderer->OffCustomDepth(PostProcess::BLOOM, i);

                                }

                                ReflectFields->CustomDepth[i] = customDepths[i];
                                ImGui::EndCombo();
                            }
                        }
                    }
                    ImGui::EndTable();
                }
                ImGui::TreePop();
            }

            ImGui::PopID();
        }

        if (ImGui::Button("Apply##MeshComponent", ImVec2(100, 50)))
        {
        }

        ImGui::TreePop();
    }    
}

void MeshComponent::InitMaterial()
{
    const auto& model     = Renderer->GetModel();
    auto&       materials = model->GetMaterials();
    size_t      meshCount = model->GetMeshCount();

    if (ReflectFields->CustomDepth.size() <= meshCount)
    {
        ReflectFields->BlendMode.resize(meshCount, 0);
        ReflectFields->CullMode.resize(meshCount, 0);
        ReflectFields->IsTwoSided.resize(meshCount, false);
        ReflectFields->CustomDepth.resize(meshCount, PostProcess::BLOOM);
    }

    for (size_t i = 0; i < meshCount; i++)
    {
        Renderer->OnCustomDepth(ReflectFields->CustomDepth[i], (UINT)i);

        //materials[i].ShadingModel = (Material::ShadingModelType)ReflectFields->ShadingModel[i];
        materials[i].BlendMode    = (Material::BlendModeType)ReflectFields->BlendMode[i];
        materials[i].CullMode     = (Material::CullModeType)ReflectFields->CullMode[i];
        materials[i].IsTwoSided   = ReflectFields->IsTwoSided[i];
    }

    Renderer->OffCustomDepth(PostProcess::OUTLINE);
}
