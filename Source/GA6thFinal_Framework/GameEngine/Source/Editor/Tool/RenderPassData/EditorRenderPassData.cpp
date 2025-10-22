#include "pch.h"
#include "EditorRenderPassData.h"
#include "Engine/GraphicsCore/RenderPassDataHelper.h"

void CenterText(const char* text)
{
    if (text == nullptr) return;
    float columnWidth = ImGui::GetColumnWidth();
    float textWidth = ImGui::CalcTextSize(text).x;
    ImGui::SetCursorPosX(ImGui::GetCursorPosX() + (columnWidth - textWidth) * 0.5f);
    ImGui::TextUnformatted(text);
}

EditorRenderPassData::EditorRenderPassData()
{
    SetLabel("RenderPassData");
    SetDockLayout(ImGuiDir_Right);
}

EditorRenderPassData::~EditorRenderPassData() = default;

void EditShadowProperty(std::any& property)
{
    auto& shadowProps = std::any_cast<ShadowPassProperty&>(property);

    if (ImGui::BeginTable("Shadow Properties", 2, ImGuiTableFlags_Borders | ImGuiTableFlags_RowBg))
    {
        ImGui::TableNextRow();
        ImGui::TableSetColumnIndex(0);
        CenterText("Near Plane");
        ImGui::TableSetColumnIndex(1);
        ImGui::PushItemWidth(-FLT_MIN);
        ImGui::DragFloat("##NearPlane", &shadowProps.NearPlane, 0.01f, 0.01f, 100.0f, "%.2f");
        ImGui::PopItemWidth();

        ImGui::TableNextRow();
        ImGui::TableSetColumnIndex(0);
        CenterText("Far Plane");
        ImGui::TableSetColumnIndex(1);
        ImGui::PushItemWidth(-FLT_MIN);
        ImGui::DragFloat("##FarPlane", &shadowProps.FarPlane, 0.1f, 0.1f, 10000.0f, "%.1f");
        ImGui::PopItemWidth();

        ImGui::TableNextRow();
        ImGui::TableSetColumnIndex(0);
        CenterText("Split Factor");
        ImGui::TableSetColumnIndex(1);
        ImGui::PushItemWidth(-FLT_MIN);
        ImGui::SliderFloat("##SplitFactor", &shadowProps.SplitFactor, 0.0f, 1.0f, "%.2f");
        ImGui::PopItemWidth();

        ImGui::EndTable();
    }
}

void EditBloomProperty(std::any& property)
{
    auto& bloomProps = std::any_cast<BloomPassProperty&>(property);

    if (ImGui::BeginTable("Bloom Properties", 2, ImGuiTableFlags_Borders | ImGuiTableFlags_RowBg))
    {
        ImGui::TableNextRow();
        ImGui::TableSetColumnIndex(0);
        CenterText("Threshold");
        ImGui::TableSetColumnIndex(1);
        ImGui::PushItemWidth(-FLT_MIN);
        ImGui::DragFloat("##Threshold", &bloomProps.Threshold, 0.01f, 0.0f, 100.0f);
        ImGui::PopItemWidth();

        ImGui::TableNextRow();
        ImGui::TableSetColumnIndex(0);
        CenterText("Intensity");
        ImGui::TableSetColumnIndex(1);
        ImGui::PushItemWidth(-FLT_MIN);
        ImGui::DragFloat("##Intensity", &bloomProps.Intensity, 0.01f, 0.0f, 100.0f);
        ImGui::PopItemWidth();

        ImGui::TableNextRow();
        ImGui::TableSetColumnIndex(0);
        CenterText("Bloom Knee");
        ImGui::TableSetColumnIndex(1);
        ImGui::PushItemWidth(-FLT_MIN);
        ImGui::DragFloat("##BloomKnee", &bloomProps.BloomKnee, 0.01f, 0.0f, 100.0f);
        ImGui::PopItemWidth();

        ImGui::EndTable();
    }
}

void EditToneMappingProperty(std::any& property)
{
    auto& toneMappingProps = std::any_cast<ToneMappingProperty&>(property);

    if (ImGui::BeginTable("Tone Mapping Properties", 2, ImGuiTableFlags_Borders | ImGuiTableFlags_RowBg))
    {
        ImGui::TableNextRow();
        ImGui::TableSetColumnIndex(0);
        CenterText("Exposure");
        ImGui::TableSetColumnIndex(1);
        ImGui::PushItemWidth(-FLT_MIN);
        ImGui::DragFloat("##Exposure", &toneMappingProps.Exposure, 0.01f, -10.f, 10.0f);
        ImGui::PopItemWidth();

        ImGui::TableNextRow();
        ImGui::TableSetColumnIndex(0);
        CenterText("Saturation");
        ImGui::TableSetColumnIndex(1);
        ImGui::PushItemWidth(-FLT_MIN);
        ImGui::DragFloat("##Saturation", &toneMappingProps.Saturation, 0.01f, 0.0f, 2.0f);
        ImGui::PopItemWidth();

        ImGui::TableNextRow();
        ImGui::TableSetColumnIndex(0);
        CenterText("Contrast");
        ImGui::TableSetColumnIndex(1);
        ImGui::PushItemWidth(-FLT_MIN);
        ImGui::DragFloat("##Contrast", &toneMappingProps.Contrast, 0.01f, 0.0f, 2.0f);
        ImGui::PopItemWidth();

        ImGui::TableNextRow();
        ImGui::TableSetColumnIndex(0);
        CenterText("White Balance");
        ImGui::TableSetColumnIndex(1);
        ImGui::PushItemWidth(-FLT_MIN);
        ImGui::ColorEdit3("##WhiteBalance", &toneMappingProps.WhiteBalance.x, ImGuiColorEditFlags_Float);
        ImGui::PopItemWidth();

        ImGui::EndTable();
    }
}

void EditSSAOProperty(std::any& property)
{
    auto& ssaoProps = std::any_cast<SSAOPassProperty&>(property);

    if (ImGui::BeginTable("SSAO Properties", 2, ImGuiTableFlags_Borders | ImGuiTableFlags_RowBg))
    {
        ImGui::TableNextRow();
        ImGui::TableSetColumnIndex(0);
        CenterText("Radius");
        ImGui::TableSetColumnIndex(1);
        ImGui::PushItemWidth(-FLT_MIN);
        ImGui::DragFloat("##Radius", &ssaoProps.Radius, 0.01f, 0.f, 50.f);
        ImGui::PopItemWidth();

        ImGui::TableNextRow();
        ImGui::TableSetColumnIndex(0);
        CenterText("FallOff");
        ImGui::TableSetColumnIndex(1);
        ImGui::PushItemWidth(-FLT_MIN);
        ImGui::DragFloat("##FallOff", &ssaoProps.Falloff, 0.001f, 0.f, 5.0f);
        ImGui::PopItemWidth();

        ImGui::TableNextRow();
        ImGui::TableSetColumnIndex(0);
        CenterText("StrengthFactor");
        ImGui::TableSetColumnIndex(1);
        ImGui::PushItemWidth(-FLT_MIN);
        ImGui::DragFloat("##StrengthFactor", &ssaoProps.StrengthFactor, 0.01f, 0.1f, 5.f);
        ImGui::PopItemWidth();

        ImGui::TableNextRow();
        ImGui::TableSetColumnIndex(0);
        CenterText("ContrastFactor");
        ImGui::TableSetColumnIndex(1);
        ImGui::PushItemWidth(-FLT_MIN);
        ImGui::DragFloat("##ContrastFactor", &ssaoProps.ContrastFactor, 0.01f, 0.1f, 5.f);
        ImGui::PopItemWidth();

        ImGui::TableNextRow();
        ImGui::TableSetColumnIndex(0);
        CenterText("Threshold");
        ImGui::TableSetColumnIndex(1);
        ImGui::PushItemWidth(-FLT_MIN);
        ImGui::DragFloat("##Threshold", &ssaoProps.Threshold, 0.0005f, 0.f, 1.f);
        ImGui::PopItemWidth();

        ImGui::EndTable();
    }
}

void EditSSRProperty(std::any& property)
{
    auto& ssrProps = std::any_cast<SSRPassProperty&>(property);

    if (ImGui::BeginTable("SSR Properties", 2, ImGuiTableFlags_Borders | ImGuiTableFlags_RowBg))
    {
        ImGui::TableNextRow();
        ImGui::TableSetColumnIndex(0);
        CenterText("Max Thickness");
        ImGui::TableSetColumnIndex(1);
        ImGui::PushItemWidth(-FLT_MIN);
        ImGui::DragFloat("##MaxThickness", &ssrProps.MaxThickness, 0.01f, 0.01f, 10.f);
        ImGui::PopItemWidth();

        ImGui::TableNextRow();
        ImGui::TableSetColumnIndex(0);
        CenterText("Step Size");
        ImGui::TableSetColumnIndex(1);
        ImGui::PushItemWidth(-FLT_MIN);
        ImGui::DragFloat("##StepSize", &ssrProps.StepSize, 0.01f, 0.01f, 10.f);
        ImGui::PopItemWidth();

        ImGui::TableNextRow();
        ImGui::TableSetColumnIndex(0);
        CenterText("Max Ray Count");
        ImGui::TableSetColumnIndex(1);
        ImGui::PushItemWidth(-FLT_MIN);
        ImGui::DragFloat("##MaxRayCount", &ssrProps.MaxRayCount, 1.f, 32.f, 200.f);
        ImGui::PopItemWidth();

        ImGui::TableNextRow();
        ImGui::TableSetColumnIndex(0);
        CenterText("Screen Fade");
        ImGui::TableSetColumnIndex(1);
        ImGui::PushItemWidth(-FLT_MIN);
        ImGui::DragFloat("##ScreenFade", &ssrProps.ScreenFade, 0.01f, 0.01f, 10.f);
        ImGui::PopItemWidth();

        ImGui::EndTable();
    }
}

void EditParallaxMappingProperty(std::any& property)
{
    auto& parallaxProps = std::any_cast<ParallaxMappingProperty&>(property);

    if (ImGui::BeginTable("Parallax Mapping Properties", 2, ImGuiTableFlags_Borders | ImGuiTableFlags_RowBg))
    {
        ImGui::TableNextRow();
        ImGui::TableSetColumnIndex(0);
        CenterText("Height Scale");
        ImGui::TableSetColumnIndex(1);
        ImGui::PushItemWidth(-FLT_MIN);
        ImGui::DragFloat("##HeightScale", &parallaxProps.HeightScale, 0.001f, 0.0f, 5.f);
        ImGui::PopItemWidth();

        ImGui::TableNextRow();
        ImGui::TableSetColumnIndex(0);
        CenterText("Mip Bias");
        ImGui::TableSetColumnIndex(1);
        ImGui::PushItemWidth(-FLT_MIN);
        ImGui::DragFloat("##MipBias", &parallaxProps.MipBias, 0.01f, 0.f, 15.f);
        ImGui::PopItemWidth();

        ImGui::EndTable();
    }
}
void EditVolumetricFogProperty(std::any& property)
{
    auto& fogProperty = std::any_cast<VolumetricFogProperty&>(property);

    if (ImGui::BeginTable("Volumetric Fog Properties", 2, ImGuiTableFlags_Borders | ImGuiTableFlags_RowBg))
    {
        ImGui::TableNextRow();
        ImGui::TableSetColumnIndex(0);
        CenterText("Custom Far");
        ImGui::TableSetColumnIndex(1);
        ImGui::PushItemWidth(-FLT_MIN);
        ImGui::DragFloat("##CustomFar", &fogProperty.CustomFar, 1.f, 1.f, 1000.f);
        ImGui::PopItemWidth();

        ImGui::TableNextRow();
        ImGui::TableSetColumnIndex(0);
        CenterText("Fog Anisotropy");
        ImGui::TableSetColumnIndex(1);
        ImGui::PushItemWidth(-FLT_MIN);
        ImGui::DragFloat("##FogAnisotropy", &fogProperty.FogAnisotropy, 0.001f, 0.001f, 1.f);
        ImGui::PopItemWidth();

        ImGui::TableNextRow();
        ImGui::TableSetColumnIndex(0);
        CenterText("Light Shaft Anisotropy");
        ImGui::TableSetColumnIndex(1);
        ImGui::PushItemWidth(-FLT_MIN);
        ImGui::DragFloat("##LightShaftAnisotropy", &fogProperty.LightShaftAnisotropy, 0.001f, 0.001f, 1.f);
        ImGui::PopItemWidth();

        ImGui::TableNextRow();
        ImGui::TableSetColumnIndex(0);
        CenterText("Density");
        ImGui::TableSetColumnIndex(1);
        ImGui::PushItemWidth(-FLT_MIN);
        ImGui::DragFloat("##Density", &fogProperty.Density, 0.01f, 0.1f, 10.f);
        ImGui::PopItemWidth();

        ImGui::TableNextRow();
        ImGui::TableSetColumnIndex(0);
        CenterText("Strength");
        ImGui::TableSetColumnIndex(1);
        ImGui::PushItemWidth(-FLT_MIN);
        ImGui::DragFloat("##Strength", &fogProperty.Strength, 0.1f, 1.f, 100.f);
        ImGui::PopItemWidth();

        ImGui::TableNextRow();
        ImGui::TableSetColumnIndex(0);
        CenterText("Blend With Scene");
        ImGui::TableSetColumnIndex(1);
        ImGui::PushItemWidth(-FLT_MIN);
        ImGui::DragFloat("##BlendWithScene", &fogProperty.BlendWithScene, 0.001f, 0.001f, 1.f);
        ImGui::PopItemWidth();

        ImGui::TableNextRow();
        ImGui::TableSetColumnIndex(0);
        CenterText("Blend With Prev Frame");
        ImGui::TableSetColumnIndex(1);
        ImGui::PushItemWidth(-FLT_MIN);
        ImGui::DragFloat("##BlendWithPrevFrame", &fogProperty.BlendWithPrevFrame, 0.001f, 0.001f, 1.f);
        ImGui::PopItemWidth();

        ImGui::TableNextRow();
        ImGui::TableSetColumnIndex(0);
        CenterText("Fog Intensity");
        ImGui::TableSetColumnIndex(1);
        ImGui::PushItemWidth(-FLT_MIN);
        ImGui::DragFloat("##FogIntensity", &fogProperty.FogIntensity, 0.01f, 0.f, 5.f);
        ImGui::PopItemWidth();

        ImGui::TableNextRow();
        ImGui::TableSetColumnIndex(0);
        CenterText("Light Shaft Intensity");
        ImGui::TableSetColumnIndex(1);
        ImGui::PushItemWidth(-FLT_MIN);
        ImGui::DragFloat("##LightShaftIntensity", &fogProperty.LightShaftIntensity, 0.01f, 0.f, 5.f);
        ImGui::PopItemWidth();

        ImGui::TableNextRow();
        ImGui::TableSetColumnIndex(0);
        CenterText("Fog Color");
        ImGui::TableSetColumnIndex(1);
        ImGui::PushItemWidth(-FLT_MIN);
        ImGui::ColorEdit3("##FogColor", fogProperty.FogColor);
        ImGui::PopItemWidth();

        ImGui::EndTable();
    }
}

void EditSSGIProperty(std::any& property)
{
    auto& giProperty = std::any_cast<SSGIProperty&>(property);

    if (ImGui::BeginTable("SSGI Properties", 2, ImGuiTableFlags_Borders | ImGuiTableFlags_RowBg))
    {
        ImGui::TableNextRow();
        ImGui::TableSetColumnIndex(0);
        CenterText("Radius");
        ImGui::TableSetColumnIndex(1);
        ImGui::PushItemWidth(-FLT_MIN);
        ImGui::DragFloat("##Radius", &giProperty.Radius, 0.01f, 0.1f, 4.f);
        ImGui::PopItemWidth();

        ImGui::TableNextRow();
        ImGui::TableSetColumnIndex(0);
        CenterText("Thickness");
        ImGui::TableSetColumnIndex(1);
        ImGui::PushItemWidth(-FLT_MIN);
        ImGui::DragFloat("##Thickness", &giProperty.Thickness, 0.001f, 0.01f, 150.f);
        ImGui::PopItemWidth();

        ImGui::TableNextRow();
        ImGui::TableSetColumnIndex(0);
        CenterText("NumSample");
        ImGui::TableSetColumnIndex(1);
        ImGui::PushItemWidth(-FLT_MIN);
        ImGui::DragInt("##NumSample", &giProperty.NumSample, 1, 1, 16);
        ImGui::PopItemWidth();

        ImGui::TableNextRow();
        ImGui::TableSetColumnIndex(0);
        CenterText("Intensity");
        ImGui::TableSetColumnIndex(1);
        ImGui::PushItemWidth(-FLT_MIN);
        ImGui::DragFloat("##Intensity", &giProperty.Intensity, 0.1f, 0.0f, 500.f);
        ImGui::PopItemWidth();

        ImGui::TableNextRow();
        ImGui::TableSetColumnIndex(0);
        CenterText("TemporalWeight");
        ImGui::TableSetColumnIndex(1);
        ImGui::PushItemWidth(-FLT_MIN);
        ImGui::DragFloat("##TemporalWeight", &giProperty.TemporalWeight, 0.01f, 0.7f, 0.95f);
        ImGui::PopItemWidth();

        ImGui::TableNextRow();
        ImGui::TableSetColumnIndex(0);
        CenterText("DepthSigma");
        ImGui::TableSetColumnIndex(1);
        ImGui::PushItemWidth(-FLT_MIN);
        ImGui::DragFloat("##DepthSigma", &giProperty.DepthSigma, 0.01f, 0.5f, 5.f);
        ImGui::PopItemWidth();

        ImGui::TableNextRow();
        ImGui::TableSetColumnIndex(0);
        CenterText("NormalSigma");
        ImGui::TableSetColumnIndex(1);
        ImGui::PushItemWidth(-FLT_MIN);
        ImGui::DragFloat("##NormalSigma", &giProperty.NormalSigma, 1.f, 16.f, 256.f);
        ImGui::PopItemWidth();

        ImGui::EndTable();
    }
}

void EditFXAAProperty(std::any& property)
{
    auto& fxaaProps = std::any_cast<FXAAProperty&>(property);

    if (ImGui::BeginTable("Tone Mapping Properties", 2, ImGuiTableFlags_Borders | ImGuiTableFlags_RowBg))
    {
        ImGui::TableNextRow();
        ImGui::TableSetColumnIndex(0);
        CenterText("QualitySubpixel");
        ImGui::TableSetColumnIndex(1);
        ImGui::PushItemWidth(-FLT_MIN);
        ImGui::DragFloat("##QualitySubpixel", &fxaaProps.QualitySubpixel, 0.001f, 0.f, 1.f);
        ImGui::PopItemWidth();

        ImGui::TableNextRow();
        ImGui::TableSetColumnIndex(0);
        CenterText("QualityEdgeDetectionThreshold");
        ImGui::TableSetColumnIndex(1);
        ImGui::PushItemWidth(-FLT_MIN);
        ImGui::DragFloat("##QualityEdgeDetectionThreshold", &fxaaProps.QualityEdgeDetectionThreshold, 0.001f, 0.0f, 1.0f);
        ImGui::PopItemWidth();

        ImGui::TableNextRow();
        ImGui::TableSetColumnIndex(0);
        CenterText("QualityMinimumEdgeThreshold");
        ImGui::TableSetColumnIndex(1);
        ImGui::PushItemWidth(-FLT_MIN);
        ImGui::DragFloat("##QualityMinimumEdgeThreshold", &fxaaProps.QualityMinimumEdgeThreshold, 0.001f, 0.0f, 1.0f);
        ImGui::PopItemWidth();

        ImGui::EndTable();
    }
}


void EditorRenderPassData::OnFrameRender()
{
    if (ImGui::TreeNodeEx("Properties"))
    {
        auto& renderPassProperties = UmGraphics.GetRenderPassProperties();
        for (auto& [passName, property] : renderPassProperties)
        {
            if (ImGui::TreeNodeEx(passName.c_str()))
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
                else if (property.type() == typeid(SSGIProperty))
                {
                    EditSSGIProperty(property);
                }
                else if (property.type() == typeid(FXAAProperty))
                {
                    EditFXAAProperty(property);
                }
                ImGui::TreePop();
            }
        }
        ImGui::TreePop();
    }

    if (ImGui::TreeNodeEx("Images"))
    {
        const auto& renderPassImages = UmGraphics.GetRenderPassImages();
        for (const auto& [sceneName, passes] : renderPassImages)
        {
            if (ImGui::TreeNodeEx((sceneName).c_str()))
            {
                for (const auto& [passName, images] : passes)
                {
                    if (ImGui::TreeNodeEx(passName.c_str()))
                    {
                        for (const auto& [dataName, handles] : images)
                        {
                            if (ImGui::TreeNodeEx(dataName.c_str()))
                            {
                                for (const auto& handle : handles)
                                {
                                    ImVec2 availSize = ImGui::GetContentRegionAvail();
                                    ImGui::Image((ImTextureID)handle.ptr,
                                                 ImVec2(availSize.x * 0.5f, availSize.x * 0.5f));
                                }
                                ImGui::TreePop();
                            }
                        }
                        ImGui::TreePop();
                    }
                }
                ImGui::TreePop();
            }
        }
        ImGui::TreePop();
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