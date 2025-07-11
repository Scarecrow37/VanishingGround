#pragma once
class ParticleEffectSerializer : public File::FileEventSubscriber
{
public:
    ParticleEffectSerializer() {};
    virtual ~ParticleEffectSerializer() {}

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
    void                    Serialize(class ParticleEffect* effect, File::Path destPath);
    class ParticleEffect*   Deserialize(File::Path filePath, bool isEditor);
    void                    PreDeserialize(File::Path filePath);
    std::vector<File::Path> GetUsedTexturePaths() { return UsedTexturePaths; }
    std::vector<File::Path> GetUsedModelPaths() const { return UsedModelPaths; }

private:
    std::vector<File::Path> UsedTexturePaths;
    std::vector<File::Path> UsedModelPaths;
};
