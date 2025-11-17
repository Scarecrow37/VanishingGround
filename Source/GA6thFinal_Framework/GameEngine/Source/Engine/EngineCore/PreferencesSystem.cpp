#include "pch.h"
#include "PreferencesSystem.h"
#include "../GraphicsEngine/GraphicsController.h"

PreferencesSystem::PreferencesSystem() {}

PreferencesSystem::~PreferencesSystem() {}

void PreferencesSystem::Initialize()
{
    _graphicsController = std::make_unique<class GraphicsController>();
    LoadData();
}

void PreferencesSystem::Uninitialize()
{
    SaveData();
}

void PreferencesSystem::SetSSR(bool enable) 
{
    _graphicsController->SetSSR("Game", enable);
    _onSSR = enable;
}
void PreferencesSystem::SetSSR(std::string_view sceneName, bool enable)
{
    _graphicsController->SetSSR(sceneName, enable);
}

void PreferencesSystem::SetSSAO(bool enable)
{
    _graphicsController->SetSSAO("Game", enable);
    _onSSAO = enable;
}

void PreferencesSystem::SetSSAO(std::string_view sceneName, bool enable) 
{
    _graphicsController->SetSSAO(sceneName, enable);
}

void PreferencesSystem::SetVolumetricFog(bool enable) 
{
    _graphicsController->SetVolumetricFog("Game", enable);
    _onVolumFog = enable;
}

void PreferencesSystem::SetVolumetricFog(std::string_view sceneName, bool enable) 
{
    _graphicsController->SetVolumetricFog(sceneName, enable);
}

void PreferencesSystem::SetBloom(bool enable) 
{
    _graphicsController->SetBloom("Game", enable);
    _onBloom = enable;
}

void PreferencesSystem::SetBloom(std::string_view sceneName, bool enable) 
{
    _graphicsController->SetBloom(sceneName, enable);
}

void PreferencesSystem::SetRayTracing(bool enable) {}

void PreferencesSystem::SetRayTracing(std::string_view sceneName, bool enable) {}

void PreferencesSystem::SetTextureQuality(TextureQuality quality) 
{
    float bias = 0.f;
    switch (quality)
    {
    case PreferencesSystem::TextureQuality::LOW:
        bias = 3.f;
        _textureQuality = 0;
        break;
    case PreferencesSystem::TextureQuality::MEDIUM:
        bias = 1.5f;
        _textureQuality = 1;
        break;
    case PreferencesSystem::TextureQuality::HIGH:
        bias = 0.f;
        _textureQuality = 2;
        break;
    default:
        break;
    }
    _graphicsController->SetTextureQuality("Game", bias);
}

void PreferencesSystem::SetTextureQuality(std::string_view sceneName, TextureQuality quality) 
{
    float bias = 0.f;
    switch (quality)
    {
    case PreferencesSystem::TextureQuality::LOW:
        bias = 3.f;
        break;
    case PreferencesSystem::TextureQuality::MEDIUM:
        bias = 1.5f;
        break;
    case PreferencesSystem::TextureQuality::HIGH:
        bias = 0.f;
        break;
    default:
        break;
    }
    _graphicsController->SetTextureQuality(sceneName, bias);
}

void PreferencesSystem::SetShadowQuality(int quality) {}

void PreferencesSystem::SetShadowQuality(std::string_view sceneName, int quality) {}

void PreferencesSystem::SetMasterVolume(float value, float maxVolume) 
{
    assert(0 < maxVolume && L"Max Volume must bigger than 0");
    // 0~1의값
    _masterVolume = value / maxVolume;
    _masterVolume = std::clamp(_masterVolume, 0.f, 1.f);

    UmAudio.SetVolume(_masterVolume);
}

void PreferencesSystem::SetBGMVolume(float value, float maxVolume) 
{
    assert(0 < maxVolume && L"Max Volume must bigger than 0");
    // 0~1의값
    _BGMVolume = value / maxVolume;
    _BGMVolume = std::clamp(_BGMVolume, 0.f, 1.f);

    UmAudio.SetVolume(Audio::GROUP_BGM, _BGMVolume);
}

void PreferencesSystem::SetSFXVolume(float value, float maxVolume) 
{
    assert(0 < maxVolume && L"Max Volume must bigger than 0");
    // 0~1의값
    _SFXVolume = value / maxVolume;
    _SFXVolume = std::clamp(_SFXVolume, 0.f, 1.f);

    UmAudio.SetVolume(Audio::GROUP_EFFECT, _SFXVolume);
}

void PreferencesSystem::SaveData()
{
    std::filesystem::path configPath = GetFilePath();
    std::filesystem::create_directories(configPath.parent_path());

    std::ofstream outFile(configPath);
    if (!outFile.is_open())
    {
        return;
    }
    outFile << "# Graphics Settings\n";
    outFile << "SSR : " << _onSSR << "\n";
    outFile << "SSAO : " << _onSSAO << "\n";
    outFile << "Bloom : " << _onBloom << "\n";
    outFile << "VolumetricFog : " << _onVolumFog << "\n";
    outFile << "TextureQuality : " << _textureQuality << "\n";
    outFile << "OnRayTracing : " << _onRayTracing << "\n\n";

    outFile << "# Audio Settings (0.0 ~ 1.0)\n";
    outFile << "MasterVolume : " << _masterVolume << "\n";
    outFile << "SFXVolume : " << _SFXVolume << "\n";
    outFile << "BGMVolume : " << _BGMVolume << "\n";

    outFile.close();
}

void PreferencesSystem::LoadData() 
{
    std::filesystem::path configPath = GetFilePath();

    if (!std::filesystem::exists(configPath))
    {
        SaveData();
        return;
    }

    std::ifstream infile(configPath);
    if (!infile.is_open())
        return;

    std::string line;
    while (std::getline(infile, line))
    {
        if (line.empty() || line[0] == '#' || line[0] == ';')
            continue;

        std::stringstream ss(line);
        std::string       key, colon;
        std::string       value;

        ss >> key;
        ss >> colon;
        ss >> value;

        if (key == "SSR")
            _onSSR = (value == "1");
        else if (key == "SSAO")
            _onSSAO = (value == "1");
        else if (key == "Bloom")
            _onBloom = (value == "1");
        else if (key == "VolumetricFog")
            _onVolumFog = (value == "1");
        else if (key == "TextureQuality")
            _textureQuality = std::stoi(value);
        else if (key == "OnRayTracing")
            _onRayTracing = (value == "1");
        else if (key == "MasterVolume")
            _masterVolume = std::stof(value);
        else if (key == "SFXVolume")
            _SFXVolume = std::stof(value);
        else if (key == "BGMVolume")
            _BGMVolume = std::stof(value);
    }
    infile.close();
}

std::filesystem::path PreferencesSystem::GetFilePath()
{
    PWSTR documentsPath = nullptr;
    SHGetKnownFolderPath(FOLDERID_Documents, 0, NULL, &documentsPath);
    std::filesystem::path filePath = documentsPath;
    CoTaskMemFree(documentsPath);

    filePath /= L"Vanishing Ground";
    filePath /= L"Preferences.inl";
    return filePath;
}