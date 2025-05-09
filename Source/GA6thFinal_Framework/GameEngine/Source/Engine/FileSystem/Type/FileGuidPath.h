#pragma once

namespace File
{
    namespace fs    = std::filesystem;

    class Path;
    class Guid;
    using FString = fs::path;

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
        bool       IsNull() const;
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
        bool       IsNull() const;
    public:
        operator File::Guid() const;
        File::Path operator+(const File::FString& v);
        File::Path operator/(const File::FString& v);
    };

    inline static const File::Guid NULL_GUID = L"";
    inline static const File::Path NULL_PATH = L"";
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
