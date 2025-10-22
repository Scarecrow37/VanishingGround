#include "pch.h"
#include "PreferencesSystem.h"
#include "../GraphicsEngine/GraphicsController.h"

PreferencesSystem::PreferencesSystem() {}

PreferencesSystem::~PreferencesSystem() {}

void PreferencesSystem::Initialize()
{
    UmFileSystem.RegisterFileEventSubscriber(this, {".ini"});
    _graphicsController = std::make_unique<class GraphicsController>();
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

void PreferencesSystem::OnPostRequestedSave()
{
    auto filePath = UmFileSystem.GetBuildSettingPath();
    filePath /= "Preferences.inl";
    
    std::ofstream outFile(filePath);

    if (!outFile.is_open())
    {
        return;
    }
    outFile << "\n";   
    outFile << "SSR : " << _onSSR << "\n";
    outFile << "SSAO : " << _onSSAO << "\n";
    outFile << "Bloom : " << _onBloom << "\n";
    outFile << "VolumetricFog : " << _onVolumFog << "\n";
    outFile << "TextureQuality : " << _textureQuality << "\n";
    outFile << "MasterVolume : " << _masterVolume << "\n";
    outFile << "SFXVolume : " << _SFXVolume << "\n";
    outFile << "BGMVolume : " << _BGMVolume;
    outFile.close();
}

void PreferencesSystem::OnPostRequestedLoad() 
{
    auto filePath = UmFileSystem.GetBuildSettingPath();
    filePath /= "Preferences.inl";

    std::ifstream infile(filePath);
    if (!infile.is_open())
        return;

    std::string line;
    while (std::getline(infile, line))
    {
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
        else if (key == "MasterVolume")
            _masterVolume = std::stof(value);
        else if (key == "SFXVolume")
            _SFXVolume = std::stof(value);
        else if (key == "BGMVolume")
            _BGMVolume = std::stof(value);
                  
    }
    infile.close();

}
