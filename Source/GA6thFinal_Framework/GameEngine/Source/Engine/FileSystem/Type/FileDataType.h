#pragma once

namespace File
{
    // stdfs
    namespace stdfs = std::filesystem;
    using FString = stdfs::path;

    class Path;
    class Guid;

    // Guid와 Path는 내부적으로 std::filesystem::path으로 구현되어 있습니다.
    // Operator를 통해 Guid -> Path 전환이 지원됩니다.
    class Guid : public FString
    {
    public:
        Guid() : FString() {}
        Guid(const FString& id) : FString(id) {}
        using FString::FString;
    public:
        File::Path ToPath() const;
    public:
        operator File::Path() const;
    };

    class Path : public FString
    {
    public:
        Path() : FString() {}
        Path(const FString& path) : FString(path) {}
        using FString::FString;
    public:
        File::Guid ToGuid() const;
    public:
        operator File::Guid() const;
        File::Path operator+(const File::FString& v);
        File::Path operator/(const File::FString& v);
    };

    inline static const File::Guid NULL_GUID;
    inline static const File::Path NULL_PATH;

    class FileData
    {
    protected:
        virtual bool Write(YAML::Node& node) const = 0;
        virtual bool Read(YAML::Node& node) const  = 0;

    public:
        bool FileCreate(bool isHidden = false) const;
        bool FileRemove() const;

        bool Create(const File::Path& path, bool isHidden = false);
        bool Load(const Path& path);
        bool Move(const Path& path);
        bool Clear();

        bool IsNull() const;

    public:
        inline const auto& GetGuid() const { return _fileGuid; }
        inline const auto& GetPath() const { return _filePath; }

    protected:
        File::Path _filePath; // 파일 경로
        File::Guid _fileGuid; // 파일 ID

        inline static const char* FILE_GUID_HEADER = "Guid";
    };

    /*
    MetaData는 파일의 메타데이터를 만들거나, 불러와서 사용하는 구조체입니다.
    */
    class MetaData : public FileData
    {
    private:
        virtual bool Write(YAML::Node& node) const override;
        virtual bool Read(YAML::Node& node) const override;
    };

    class ProjectData : public FileData
    {
    private:
        virtual bool Write(YAML::Node& node) const override;
        virtual bool Read(YAML::Node& node) const override;
    };

    struct SystemSetting
    {
        int         DebugLevel = 0; // 디버그 레벨
        std::string MetaExt    = ".UmMeta";
    };
} // namespace File

namespace std
{
    template <>
    struct hash<File::Guid>
    {
        size_t operator()(const File::Guid& guid) const
        {
            return hash<File::FString>()(guid);
        }
    };

    template <>
    struct hash<File::Path>
    {
        size_t operator()(const File::Path& path) const
        {
            return hash<File::FString>()(path);
        }
    };
} // namespace std
