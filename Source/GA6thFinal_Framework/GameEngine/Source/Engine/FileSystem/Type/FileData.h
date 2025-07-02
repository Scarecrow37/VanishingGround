#pragma once
namespace File
{
    namespace fs = std::filesystem;

    class FileData
    {
    public:
        FileData() = default; // 기본 생성자
        FileData(File::Path);
        FileData(File::Guid);
        virtual ~FileData() = default;

    protected:
        virtual bool Write(YAML::Node& node) const = 0;
        virtual bool Read(YAML::Node& node) const  = 0;

    public:
        bool FileCreate(bool isHidden = false) const;
        bool FileRemove() const;

        bool Create(const File::Path& path, bool isEmpty = false, bool isHidden = false);
        bool Load(const Path& path);
        bool Move(const Path& path);
        bool Clear();

        bool IsNull() const;

    public:
        inline const auto& GetGuid() const { return _fileGuid; }
        inline const auto& GetPath() const { return _filePath; }

    protected:
        File::Path _filePath = NULL_PATH; // 파일 경로
        File::Guid _fileGuid = NULL_GUID; // 파일 ID

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

}
