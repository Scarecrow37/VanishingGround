#include "pch.h"
#include "EditorRenderPassData.h"
#include "Engine/GraphicsCore/RenderPassDataHelper.h"

EditorRenderPassData::EditorRenderPassData()
{
    SetLabel("RenderPassData");
    SetDockLayout(ImGuiDir_Right);
}

EditorRenderPassData::~EditorRenderPassData()
{
}

void EditShadowProperty(std::any& property)
{
    auto& shadowProps = std::any_cast<ShadowPassProperty&>(property);

    ImGui::DragFloat("Near Plane", &shadowProps.NearPlane, 0.01f, 0.01f, 100.0f);
    ImGui::DragFloat("Far Plane", &shadowProps.FarPlane, 0.1f, 0.1f, 1000.0f);
    ImGui::DragFloat("Split Factor", &shadowProps.SplitFactor, 0.01f, 0.01f, 1.0f);
}

void EditBloomProperty(std::any& property)
{
    auto& bloomProps = std::any_cast<BloomPassProperty&>(property);
    ImGui::DragFloat("Threshold", &bloomProps.Threshold, 0.01f, 0.0f, 100.0f);
    ImGui::DragFloat("Intensity", &bloomProps.Intensity, 0.01f, 0.0f, 100.0f);
    ImGui::DragFloat("Bloom Knee", &bloomProps.BloomKnee, 0.01f, 0.0f, 100.0f);
}

void EditToneMappingProperty(std::any& property)
{
    auto& toneMappingProps = std::any_cast<ToneMappingProperty&>(property);
    ImGui::DragFloat("Exposure", &toneMappingProps.Exposure, 0.01f, -10.f, 10.0f);
    ImGui::DragFloat("Saturation", &toneMappingProps.Saturation, 0.01f, 0.0f, 2.0f);
    ImGui::DragFloat("Contrast", &toneMappingProps.Contrast, 0.01f, 0.0f, 2.0f);
    ImGui::ColorEdit3("White Balance", &toneMappingProps.WhiteBalance.x, ImGuiColorEditFlags_Float);
}

void EditSSAOProperty(std::any& property)
{
    auto& ssaoProps = std::any_cast<SSAOPassProperty&>(property);
    ImGui::DragFloat("Radius", &ssaoProps.Radius, 0.01f, 0.f, 50.f);
    ImGui::DragFloat("FallOff", &ssaoProps.Falloff, 0.001f, 0.f, 5.0f);
    ImGui::DragFloat("StrengthFactor", &ssaoProps.StrengthFactor, 0.01f, 0.1f, 5.f);
    ImGui::DragFloat("ContrastFactor", &ssaoProps.ContrastFactor, 0.01f, 0.1f, 5.f);
    ImGui::DragFloat("Threshold", &ssaoProps.Threshold, 0.0005f, 0.f, 1.f);
}

void EditSSRProperty(std::any& property)
{
    auto& ssrProps = std::any_cast<SSRPassProperty&>(property);
    ImGui::DragFloat("MaxThickness", &ssrProps.MaxThickness, 0.01f, 0.01f, 10.f);
    ImGui::DragFloat("StepSize", &ssrProps.StepSize, 0.01f, 0.01f, 10.f);
    ImGui::DragFloat("MaxRayCount", &ssrProps.MaxRayCount, 1.f, 32.f, 200.f);
    ImGui::DragFloat("ScreenFade", &ssrProps.ScreenFade, 0.01f, 0.01f, 10.f);
}

void EditParallaxMappingProperty(std::any& property)
{
    auto& parallaxProps = std::any_cast<ParallaxMappingProperty&>(property);
    ImGui::DragFloat("HeightScale", &parallaxProps.HeightScale, 0.001f, 0.0f, 5.f);
}
void EditVolumetricFogProperty(std::any& property)
{
    auto& forProperty = std::any_cast<VolumetricFogProperty&>(property);
    ImGui::DragFloat("Anisotropy", &forProperty.Anisotropy, 0.0f, 0.001f, 1.f);
    ImGui::DragFloat("Density", &forProperty.Density, 0.0f, 0.1f, 10.f);
    ImGui::DragFloat("Strength", &forProperty.Strength, 0.f, 1.f, 100.f);
    ImGui::DragFloat("BlendWithScene", &forProperty.BlendWithScene, 0.0f, 0.001f, 1.f);
    ImGui::DragFloat("BlendWithPrevFrame", &forProperty.BlendWithPrevFrame, 0.0f, 0.001f, 1.f);
    ImGui::DragFloat("CustomNear", &forProperty.CustomNear, 0.01f, 0.01f, 10.f);
    ImGui::DragFloat("CustomFar", &forProperty.CustomFar, 100.f, 2.f, 10000.f);
}

void EditorRenderPassData::OnFrameRender()
{
    auto& renderPassProperties = UmGraphics.GetRenderPassProperties();

    for (auto& [sceneName, properties] : renderPassProperties)
    {
        if (ImGui::TreeNodeEx(sceneName.c_str()))
        {
            for (auto& [passName, pair] : properties)
            {
                if (ImGui::TreeNodeEx(passName.c_str()))
                {
                    auto& [property, images] = pair;
                    
                    if (ImGui::TreeNodeEx("Properties"))
                    {
                        if (property.type() == typeid(ShadowPassProperty))
                        {                            
                            EditShadowProperty(property);
                        }
                        else if (property.type() == typeid(BloomPassProperty))
                        {
                            EditBloomProperty(property);
                        }
                        else if (property.type() == typeid(ToneMappingProperty))
                        {
                            EditToneMappingProperty(property);
                        }
                        else if (property.type() == typeid(SSAOPassProperty))
                        {
                            EditSSAOProperty(property);
                        }
                        else if (property.type() == typeid(SSRPassProperty))
                        {
                            EditSSRProperty(property);
                        }
                        else if (property.type() == typeid(ParallaxMappingProperty))
                        {
                            EditParallaxMappingProperty(property);
                        }
                        else if (property.type() == typeid(VolumetricFogProperty))
                        {
                            EditVolumetricFogProperty(property);
                        }
                        ImGui::TreePop();
                    }

                    if (ImGui::TreeNodeEx("Images"))
                    {
                        for (const auto& [dataName, handles] : images)
                        {
                            if (ImGui::TreeNodeEx(dataName.c_str()))
                            {
                                for (const auto& handle : handles)
                                {
                                    ImVec2 availSize = ImGui::GetContentRegionAvail();
                                    ImGui::Image((ImTextureID)handle.ptr, ImVec2(availSize.x * 0.5f, availSize.x * 0.5f));
                                }
                                ImGui::TreePop();
                            }
                        }
                        ImGui::TreePop();
                    }
                    ImGui::TreePop();
                }
            }
            ImGui::TreePop();
        }
    }
    if (ImGui::Button("SaveData"))
    {
        std::vector<std::pair<LPCWSTR, LPCWSTR>> filters = {{L"Save Files (*.inl)", L"*.inl\0\0"}};
        File::Path exportPath;
        bool result = File::ShowSaveFileDialog(NULL, L"Export Render Pass Data", L"", L"data.inl", filters, exportPath);
        if (result)
        {
            SaveRenderPassData(exportPath.string());

            auto filePath = UmFileSystem.GetBuildSettingPath();
            if (!filePath.empty())
            {
                filePath /= "GraphicsSetting.inl";
                SaveRenderPassData(filePath.string());
            }
        }
    }
    ImGui::SameLine();
    if (ImGui::Button("LoadData"))
    {
        std::vector<std::pair<LPCWSTR, LPCWSTR>> filters = {{L"Load Files (*.inl)", L"*.inl\0\0"}};
        File::Path importPath;
        bool result = File::ShowOpenFileDialog(NULL, L"Import Render Pass Data", L"", filters, importPath);
        if (result)
        {
            LoadRenderPassData(importPath.string());
        }
    }
}