#pragma once

namespace File
{
    // stdfs
    namespace stdfs = std::filesystem;
    using FString = stdfs::path;

    class Path;
    class Guid;

    constexpr const char* NULL_GUID = "";

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

    /*
    MetaData는 파일의 메타데이터를 만들거나, 불러와서 사용하는 구조체입니다.
    */
    class MetaData
    {
    public:
        MetaData() : _filePath(""), _fileGuid(NULL_GUID), _projectGuid(NULL_GUID) {}
        ~MetaData() = default;
    public:
        bool Create(const Path& path);
        bool Load(const Path& path);
        bool Move(const Path& path);
        bool Remove();
        bool IsNull();
    public:
        inline const Guid& GetFileGuid() const { return _fileGuid; }
        inline const Guid& GetProjectGuid() const { return _projectGuid; }
    private:
        File::Path                _filePath;    // 메타 파일 경로
        File::Guid                _fileGuid;    // 파일 ID
        File::Guid                _projectGuid; // 프로젝트 ID

        inline static const char* FILE_GUID_HEADER = "File Guid";
        inline static const char* PROJ_GUID_HEADER = "Project Guid";
    public:
        inline static const char* EXTENSION = ".META";
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