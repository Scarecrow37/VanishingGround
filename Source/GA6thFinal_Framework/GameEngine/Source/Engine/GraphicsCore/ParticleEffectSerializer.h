#pragma once
constexpr char     MAGIC_NUMBER[] = "UMFX";
constexpr uint32_t MAJOR_VERSION  = 1;
constexpr uint32_t MINOR_VERSION  = 5;


class ParticleEffectSerializer : public File::FileEventSubscriber
{
    using EffectID = void*;
     
private:
    using Serializer = std::function<void(std::ofstream&, ParticleEffect*, File::Path)>;
    using Deserializer =
        std::function<ParticleEffect*(EffectID, const std::string&, std::ifstream&, bool, std::string_view)>;
    using Predeserializer = std::function<void(std::ifstream&)>;
    using VersionSet      = std::pair<uint32_t, uint32_t>;

    std::map<VersionSet, Serializer>      _serializers;
    std::map<VersionSet, Deserializer>    _deserializers;
    std::map<VersionSet, Predeserializer> _preDeserializers;

    void RegisterDeserializers();

public:
    ParticleEffectSerializer();
    virtual ~ParticleEffectSerializer() {}; 
                                                   


    void OnFileRegistered(const File::Path& path) override;
    void OnFileUnregistered(const File::Path& path) override;
    void OnRequestedSave() override;
    void OnPostRequestedSave() override;
    void OnRequestedLoad() override;
    void OnPostRequestedLoad() override;
    void OnRequestedInspect(const File::Path& path) override;
    void OnRequestedOpen(const File::Path& path) override;
    void OnRequestedCopy(const File::Path& path) override;
    void OnRequestedPaste(const File::Path& path) override;
    void OnFileModified(const File::Path& path) override;
    void OnFileRemoved(const File::Path& path) override;
    void OnFileRenamed(const File::Path& oldPath, const File::Path& newPath) override;
    void OnFileMoved(const File::Path& oldPath, const File::Path& newPath) override;

public:

    //versions
    void            Serialize(class ParticleEffect* effect, File::Path destPath);
    ParticleEffect* Deserialize(EffectID id, const std::string& keyString, File::Path filepath, bool isEditor,
                                std::string_view sceneName);
    void            PreDeserialize(File::Path filePath);

    void            Serialize_1_0(std::ofstream& os, ParticleEffect* effect, File::Path destPath);
    ParticleEffect* Deserialize_1_0(EffectID id, const std::string& keyString, std::ifstream& is, bool isEditor,
                                    std::string_view sceneName);
    void            PreDeserialize_1_0(std::ifstream& is);

    void            Serialize_1_1(std::ofstream& os, ParticleEffect* effect, File::Path destPath);
    ParticleEffect* Deserialize_1_1(EffectID id, const std::string& keyString, std::ifstream& is, bool isEditor,
                                    std::string_view sceneName);
    void            PreDeserialize_1_1(std::ifstream& is);

    void            Serialize_1_2(std::ofstream& os, ParticleEffect* effect, File::Path destPath);
    ParticleEffect* Deserialize_1_2(EffectID id, const std::string& keyString, std::ifstream& is, bool isEditor,
                                    std::string_view sceneName);
    void            PreDeserialize_1_2(std::ifstream& is);

    void            Serialize_1_3(std::ofstream& os, ParticleEffect* effect, File::Path destPath);
    ParticleEffect* Deserialize_1_3(EffectID id, const std::string& keyString, std::ifstream& is, bool isEditor,
                                    std::string_view sceneName);
    void            PreDeserialize_1_3(std::ifstream& is);

    void            Serialize_1_4(std::ofstream& os, ParticleEffect* effect, File::Path destPath);
    ParticleEffect* Deserialize_1_4(EffectID id, const std::string& keyString, std::ifstream& is, bool isEditor,
                                    std::string_view sceneName);
    void            PreDeserialize_1_4(std::ifstream& is);

    void            Serialize_1_5(std::ofstream& os, ParticleEffect* effect, File::Path destPath);
    ParticleEffect* Deserialize_1_5(EffectID id, const std::string& keyString, std::ifstream& is, bool isEditor,
                                    std::string_view sceneName);
    void            PreDeserialize_1_5(std::ifstream& is);


    std::set<File::Path> GetUsedTexturePaths() { return UsedTexturePaths; }
    std::set<File::Path> GetUsedModelPaths() const { return UsedModelPaths; }

private:
    std::set<File::Path> UsedTexturePaths;
    std::set<File::Path> UsedModelPaths;

private:



};
