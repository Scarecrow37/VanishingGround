#include <fstream>

// ShadowPassProperty를 문자열로 변환
inline void SerializeShadowProperty(std::ostream& os, const ShadowPassProperty& prop)
{
	os << "    Type = ShadowPassProperty\n";
	os << "    NearPlane = " << prop.NearPlane << "\n";
	os << "    FarPlane = " << prop.FarPlane << "\n";
	os << "    SplitFactor = " << prop.SplitFactor << "\n";
}

// BloomPassProperty를 문자열로 변환
inline void SerializeBloomProperty(std::ostream& os, const BloomPassProperty& prop)
{
	os << "    Type = BloomPassProperty\n";
	os << "    Threshold = " << prop.Threshold << "\n";
	os << "    Intensity = " << prop.Intensity << "\n";
	os << "    BloomKnee = " << prop.BloomKnee << "\n";
}

// ToneMappingProperty를 문자열로 변환
inline void SerializeToneMappingProperty(std::ostream& os, const ToneMappingProperty& prop)
{
	os << "    Type = ToneMappingProperty\n";
	os << "    Exposure = " << prop.Exposure << "\n";
	os << "    Saturation = " << prop.Saturation << "\n";
	os << "    Contrast = " << prop.Contrast << "\n";
	os << "    WhiteBalance = " << prop.WhiteBalance.x << " " << prop.WhiteBalance.y << " " << prop.WhiteBalance.z << "\n";
}

// SSAOPassProperty를 문자열로 변환
inline void SerializeSSAOPassProperty(std::ostream& os, const SSAOPassProperty& prop)
{
    os << "    Type = SSAOPassProperty\n";
    os << "    Radius = " << prop.Radius << "\n";
    os << "    Falloff = " << prop.Falloff << "\n";
    os << "    StrengthFactor = " << prop.StrengthFactor << "\n";
    os << "    ContrastFactor = " << prop.ContrastFactor << "\n";
    os << "    Threshold = " << prop.Threshold << "\n";
}

// SSRPassProperty를 문자열로 변환
inline void SerializeSSRPassProperty(std::ostream& os, const SSRPassProperty& prop)
{
    os << "    Type = SSRPassProperty\n";
    os << "    MaxThickness = " << prop.MaxThickness   << "\n";
    os << "    StepSize = " << prop.StepSize  << "\n";
    os << "    MaxRayCount = " << prop.MaxRayCount << "\n";
    os << "    ScreenFade = " << prop.ScreenFade << "\n";
}

// ParallaxMappingProperty를 문자열로 변환
inline void SerializeParallaxMappingProperty(std::ostream& os, const ParallaxMappingProperty& prop)
{
    os << "    Type = ParallaxMappingProperty\n";
    os << "    HeightScale = " << prop.HeightScale << "\n";
}

// VolumetricFogProperty를 문자열로 변환
inline void SerializeVolumetricFogProperty(std::ostream& os, const VolumetricFogProperty& prop)
{
    os << "    Type = VolumetricFogProperty\n";
    os << "    FogAnisotropy = " << prop.FogAnisotropy << "\n";
    os << "    LightShaftAnisotropy = " << prop.LightShaftAnisotropy << "\n";
    os << "    Density = " << prop.Density << "\n";
    os << "    Strength = " << prop.Strength << "\n";
    os << "    BlendWithScene = " << prop.BlendWithScene << "\n";
    os << "    BlendWithPrevFrame = " << prop.BlendWithPrevFrame << "\n";
    os << "    CustomNear = " << prop.CustomNear << "\n";
    os << "    CustomFar = " << prop.CustomFar << "\n";
    os << "    FogIntensity = " << prop.FogIntensity << "\n";
    os << "    LightShaftIntensity = " << prop.LightShaftIntensity << "\n";
    os << "    FogColor = " << prop.FogColor[0] << " " << prop.FogColor[1] << " " << prop.FogColor[2] << " "
       << prop.FogColor[3] << "\n";
}

// SSGIProperty를 문자열로 변환
inline void SerializeSSGIProperty(std::ostream& os, const SSGIProperty& prop)
{
    os << "    Type = SSGIProperty\n";
    os << "    Radius = " << prop.Radius << "\n";
    os << "    Thickness = " << prop.Thickness << "\n";
    os << "    NumSample = " << prop.NumSample << "\n";
    os << "    Intensity = " << prop.Intensity << "\n";
    os << "    TemporalWeight = " << prop.TemporalWeight << "\n";
    os << "    DepthSigma = " << prop.DepthSigma << "\n";
    os << "    NormalSigma = " << prop.NormalSigma << "\n";
}

// FXAAProperty를 문자열로 변환
inline void SerializeFXAAProperty(std::ostream& os, const FXAAProperty& prop)
{
    os << "    Type = FXAAProperty\n";
    os << "    QualitySubpixel = " << prop.QualitySubpixel << "\n";
    os << "    QualityEdgeDetectionThreshold = " << prop.QualityEdgeDetectionThreshold << "\n";
    os << "    QualityMinimumEdgeThreshold = " << prop.QualityMinimumEdgeThreshold << "\n";
}


// 문자열에서 ShadowPassProperty를 복원
inline void DeserializeShadowProperty(std::istream& is, ShadowPassProperty& prop)
{
	std::string line, key, equals;
	while (std::getline(is, line) && line.find('}') == std::string::npos)
	{
		std::stringstream ss(line);
		ss >> key >> equals;
		if (key == "NearPlane") ss >> prop.NearPlane;
		else if (key == "FarPlane") ss >> prop.FarPlane;
		else if (key == "SplitFactor") ss >> prop.SplitFactor;
	}
}

// 문자열에서 BloomPassProperty를 복원
inline void DeserializeBloomProperty(std::istream& is, BloomPassProperty& prop)
{
	std::string line, key, equals;
	while (std::getline(is, line) && line.find('}') == std::string::npos)
	{
		std::stringstream ss(line);
		ss >> key >> equals;
		if (key == "Threshold") ss >> prop.Threshold;
		else if (key == "Intensity") ss >> prop.Intensity;
		else if (key == "BloomKnee") ss >> prop.BloomKnee;
	}
}

// 문자열에서 ToneMappingProperty를 복원
inline void DeserializeToneMappingProperty(std::istream& is, ToneMappingProperty& prop)
{
	std::string line, key, equals;
	while (std::getline(is, line) && line.find('}') == std::string::npos)
	{
		std::stringstream ss(line);
		ss >> key >> equals;
		if (key == "Exposure") ss >> prop.Exposure;
		else if (key == "Saturation") ss >> prop.Saturation;
		else if (key == "Contrast") ss >> prop.Contrast;
		else if (key == "WhiteBalance")
		{
			// WhiteBalance는 Vector3로 가정
			ss >> prop.WhiteBalance.x >> prop.WhiteBalance.y >> prop.WhiteBalance.z;
		}
	}
}

// 문자열에서 SSAOPassProperty를 복원
inline void DeserializeSSAOPassProperty(std::istream& is, SSAOPassProperty& prop)
{
    std::string line, key, equals;
    while (std::getline(is, line) && line.find('}') == std::string::npos)
    {
        std::stringstream ss(line);
        ss >> key >> equals;
        if (key == "Radius")
            ss >> prop.Radius;
        else if (key == "Falloff")
            ss >> prop.Falloff;
        else if (key == "StrengthFactor")
            ss >> prop.StrengthFactor;
        else if (key == "ContrastFactor")
            ss >> prop.ContrastFactor;
        else if (key == "Threshold")
            ss >> prop.Threshold;
    }
}

// 문자열에서 SSRPassProperty를 복원
inline void DeserializeSSRPassProperty(std::istream& is, SSRPassProperty& prop)
{
    std::string line, key, equals;
    while (std::getline(is, line) && line.find('}') == std::string::npos)
    {
        std::stringstream ss(line);
        ss >> key >> equals;
        if (key == "MaxThickness")
            ss >> prop.MaxThickness;
        else if (key == "StepSize")
            ss >> prop.StepSize;
        else if (key == "MaxRayCount")
            ss >> prop.MaxRayCount;
        else if (key == "ScreenFade")
            ss >> prop.ScreenFade;
    }
}

// 문자열에서 ParallaxMappingProperty를 복원
inline void DeserializeParallaxMappingProperty(std::istream& is, ParallaxMappingProperty& prop)
{
    std::string line, key, equals;
    while (std::getline(is, line) && line.find('}') == std::string::npos)
    {
        std::stringstream ss(line);
        ss >> key >> equals;
        if (key == "HeightScale")
            ss >> prop.HeightScale;
    }
}

// 문자열에서 VolumetricFogProperty를 복원
inline void DeserializeVolumetricFogProperty(std::istream& is, VolumetricFogProperty& prop)
{
    std::string line, key, equals;
    while (std::getline(is, line) && line.find('}') == std::string::npos)
    {
        std::stringstream ss(line);
        ss >> key >> equals;
        if (key == "FogAnisotropy")
            ss >> prop.FogAnisotropy;
        else if (key == "LightShaftAnisotropy")
            ss >> prop.LightShaftAnisotropy;
        else if (key == "Density")
            ss >> prop.Density;
        else if (key == "Strength")
            ss >> prop.Strength;
        else if (key == "BlendWithScene")
            ss >> prop.BlendWithScene;
        else if (key == "BlendWithPrevFrame")
            ss >> prop.BlendWithPrevFrame;
        else if (key == "CustomNear")
            ss >> prop.CustomNear;
        else if (key == "CustomFar")
            ss >> prop.CustomFar;
        else if (key == "FogIntensity")
            ss >> prop.FogIntensity;
        else if (key == "LightShaftIntensity")
            ss >> prop.LightShaftIntensity;
        else if (key == "FogColor")
            ss >> prop.FogColor[0] >> prop.FogColor[1] >> prop.FogColor[2] >> prop.FogColor[3];
    }
}

// 문자열에서 SSGIProperty를 복원
inline void DeserializeSSGIProperty(std::istream& is, SSGIProperty& prop)
{
    std::string line, key, equals;
    while (std::getline(is, line) && line.find('}') == std::string::npos)
    {
        std::stringstream ss(line);
        ss >> key >> equals;
        if (key == "Radius")
            ss >> prop.Radius;
        else if (key == "Thickness")
            ss >> prop.Thickness;
        else if (key == "NumSample")
            ss >> prop.NumSample;
        else if (key == "Intensity")
            ss >> prop.Intensity;
        else if (key == "TemporalWeight")
            ss >> prop.TemporalWeight;
        else if (key == "DepthSigma")
            ss >> prop.DepthSigma;
        else if (key == "NormalSigma")
            ss >> prop.NormalSigma;
    }
}

// 문자열에서 FXAAProperty를 복원
inline void DeserializeFXAAProperty(std::istream& is, FXAAProperty& prop)
{
    std::string line, key, equals;
    while (std::getline(is, line) && line.find('}') == std::string::npos)
    {
        std::stringstream ss(line);
        ss >> key >> equals;
        if (key == "QualitySubpixel")
            ss >> prop.QualitySubpixel;
        else if (key == "QualityEdgeDetectionThreshold")
            ss >> prop.QualityEdgeDetectionThreshold;
        else if (key == "QualityMinimumEdgeThreshold")
            ss >> prop.QualityMinimumEdgeThreshold;
    }
}

inline void SaveRenderPassData(const std::string& filePath)
{
	std::filesystem::path path(filePath);

	path.replace_extension(".inl");

	std::ofstream outFile(path);

	if (!outFile.is_open())
	{
		return;
	}

	auto& renderPassProperties = UmGraphics.GetRenderPassProperties();
	for (const auto& [passName, property] : renderPassProperties)
	{		
		outFile << "Pass " << passName << "\n{\n";

        if (property.type() == typeid(ShadowPassProperty))
        {
            SerializeShadowProperty(outFile, std::any_cast<const ShadowPassProperty&>(property));
        }
        else if (property.type() == typeid(BloomPassProperty))
        {
            SerializeBloomProperty(outFile, std::any_cast<const BloomPassProperty&>(property));
        }
        else if (property.type() == typeid(ToneMappingProperty))
        {
            SerializeToneMappingProperty(outFile, std::any_cast<const ToneMappingProperty&>(property));
        }
        else if (property.type() == typeid(SSAOPassProperty))
        {
            SerializeSSAOPassProperty(outFile, std::any_cast<const SSAOPassProperty&>(property));
        }
        else if (property.type() == typeid(SSRPassProperty))
        {
            SerializeSSRPassProperty(outFile, std::any_cast<const SSRPassProperty&>(property));
        }
        else if (property.type() == typeid(ParallaxMappingProperty))
        {
            SerializeParallaxMappingProperty(outFile, std::any_cast<const ParallaxMappingProperty&>(property));
        }
        else if (property.type() == typeid(VolumetricFogProperty))
        {
            SerializeVolumetricFogProperty(outFile, std::any_cast<const VolumetricFogProperty&>(property));
        }
        else if (property.type() == typeid(SSGIProperty))
        {
            SerializeSSGIProperty(outFile, std::any_cast<const SSGIProperty&>(property));
        }
        else if (property.type() == typeid(FXAAProperty))
        {
            SerializeFXAAProperty(outFile, std::any_cast<const FXAAProperty&>(property));
        }
        outFile << "}\n";
    }
}

inline void LoadRenderPassData(const std::string& filePath)
{
    std::ifstream inFile(filePath);
    if (!inFile.is_open())
    {
        return;
    }

    // --- Format Detection ---
    bool isOldFormat = false;
    std::string firstLine;
    std::streampos originalPos = inFile.tellg();
    while (std::getline(inFile, firstLine))
    {
        if (!firstLine.empty())
        {
            std::stringstream ss(firstLine);
            std::string keyword;
            ss >> keyword;
            if (keyword == "Scene")
            {
                isOldFormat = true;
            }
            break;
        }
    }
    inFile.clear();
    inFile.seekg(originalPos);
    // -- End of Format Detection --

    auto& renderPassProperties = UmGraphics.GetRenderPassProperties();
    std::string line, keyword, name;

    if (isOldFormat)
    {
        // --- Old Format Parsing Logic (with "Scene Game" filter) ---
        while (std::getline(inFile, line))
        {
            std::stringstream ss(line);
            ss >> keyword >> name;

            if (keyword == "Scene")
            {
                bool isGameScene = (name == "Game");
                std::getline(inFile, line); // Skip '{' line

                while (std::getline(inFile, line) && line.find('}') == std::string::npos)
                {
                    std::stringstream pass_ss(line);
                    pass_ss >> keyword >> name;

                    if (keyword == "Pass")
                    {
                        std::string currentPassName = name;
                        std::getline(inFile, line); // Skip '{' line

                        if (isGameScene && renderPassProperties.count(currentPassName))
                        {
                            auto& property = renderPassProperties.at(currentPassName);
                            std::string typeLine, typeName;
                            std::getline(inFile, typeLine);
                            std::stringstream type_ss(typeLine);
                            type_ss >> keyword >> name >> typeName;

                            if (typeName == "ShadowPassProperty") DeserializeShadowProperty(inFile, std::any_cast<ShadowPassProperty&>(property));
                            else if (typeName == "BloomPassProperty") DeserializeBloomProperty(inFile, std::any_cast<BloomPassProperty&>(property));
                            else if (typeName == "ToneMappingProperty") DeserializeToneMappingProperty(inFile, std::any_cast<ToneMappingProperty&>(property));
                            else if (typeName == "SSAOPassProperty") DeserializeSSAOPassProperty(inFile, std::any_cast<SSAOPassProperty&>(property));
                            else if (typeName == "SSRPassProperty") DeserializeSSRPassProperty(inFile, std::any_cast<SSRPassProperty&>(property));
                            else if (typeName == "ParallaxMappingProperty") DeserializeParallaxMappingProperty(inFile, std::any_cast<ParallaxMappingProperty&>(property));
                            else if (typeName == "VolumetricFogProperty") DeserializeVolumetricFogProperty(inFile, std::any_cast<VolumetricFogProperty&>(property));
                            else if (typeName == "SSGIProperty") DeserializeSSGIProperty(inFile, std::any_cast<SSGIProperty&>(property));
                            else if (typeName == "FXAAProperty") DeserializeFXAAProperty(inFile, std::any_cast<FXAAProperty&>(property));
                        }
                        else
                        {
                            // Skip content of non-game scene passes or unknown passes
                            while (std::getline(inFile, line) && line.find('}') == std::string::npos) {}
                        }
                    }
                }
            }
        }
    }
    else
    {
        // --- New Format Parsing Logic ---
        while (std::getline(inFile, line))
        {
            std::stringstream ss(line);
            ss >> keyword >> name;

            if (keyword == "Pass")
            {
                std::string currentPassName = name;
                std::getline(inFile, line); // Skip '{' line

                if (renderPassProperties.count(currentPassName))
                {
                    auto& property = renderPassProperties.at(currentPassName);
                    std::string typeLine, typeName;
                    std::getline(inFile, typeLine);
                    std::stringstream type_ss(typeLine);
                    type_ss >> keyword >> name >> typeName;

                    if (typeName == "ShadowPassProperty") DeserializeShadowProperty(inFile, std::any_cast<ShadowPassProperty&>(property));
                    else if (typeName == "BloomPassProperty") DeserializeBloomProperty(inFile, std::any_cast<BloomPassProperty&>(property));
                    else if (typeName == "ToneMappingProperty") DeserializeToneMappingProperty(inFile, std::any_cast<ToneMappingProperty&>(property));
                    else if (typeName == "SSAOPassProperty") DeserializeSSAOPassProperty(inFile, std::any_cast<SSAOPassProperty&>(property));
                    else if (typeName == "SSRPassProperty") DeserializeSSRPassProperty(inFile, std::any_cast<SSRPassProperty&>(property));
                    else if (typeName == "ParallaxMappingProperty") DeserializeParallaxMappingProperty(inFile, std::any_cast<ParallaxMappingProperty&>(property));
                    else if (typeName == "VolumetricFogProperty") DeserializeVolumetricFogProperty(inFile, std::any_cast<VolumetricFogProperty&>(property));
                    else if (typeName == "SSGIProperty") DeserializeSSGIProperty(inFile, std::any_cast<SSGIProperty&>(property));
                    else if (typeName == "FXAAProperty") DeserializeFXAAProperty(inFile, std::any_cast<FXAAProperty&>(property));
                }
            }
        }
    }
}
