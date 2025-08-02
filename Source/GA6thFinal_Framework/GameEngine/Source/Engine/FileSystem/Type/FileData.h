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
        virtual bool Read(YAML::Node& node)  = 0;

    public:
        /// <summary>
        /// 메타 파일을 윈도우에 생성합니다.
        /// </summary>
        /// <param name="hidden">파일의 숨김 여부. 기본적으로 false</param>
        /// <returns></returns>
        bool FileCreate(bool hidden = false) const;

        /// <summary>
        /// 현재 파일을 윈도우에서 삭제합니다.
        /// </summary>
        /// <returns></returns>
        bool FileRemove() const;

        /// <summary>
        /// FileData 정보를 생성합니다. Guid를 생성합니다.
        /// </summary>
        /// <param name="path"></param>
        /// <param name="isEmpty"></param>
        /// <param name="isHidden"></param>
        /// <returns></returns>
        bool Create(const File::Path& path, bool isEmpty = false, bool isHidden = false);
        bool Load(const Path& path);
        bool Move(const Path& path);
        bool Clear();

        bool IsNull() const;

    public:
        inline const File::Path& GetPath() const { return _filePath; }
        inline const File::Guid& GetGuid() const { return _fileGuid; }

    protected:
        File::Path _filePath = NULL_PATH; // 파일 경로
        File::Guid _fileGuid = NULL_GUID; // 파일 ID

        static constexpr const char* FILE_GUID_HEADER = "Guid";
    };

    
    /// <summary>
    /// <para>MetaData:</para>
    /// <para>해당 클래스는 파일의 메타데이터를 만들거나, 불러와서 사용하는 구조체입니다.</para>
    /// <para>파일이 생성되면 Meta파일을 생성하며, 해당 파일을 삭제할 땐 Meta파일이 삭제되지 않습니다.</para>
    /// </summary>
    class MetaData : public FileData
    {
    public:

    private:
        virtual bool Write(YAML::Node& node) const override;
        virtual bool Read(YAML::Node& node) override;

    public:
        inline void SetAssetID(int assetID) { _assetID = assetID; FileCreate(); }
        inline int GetAssetID() const { return _assetID; }

    private:
        int _assetID; // 에셋 ID
        
        static constexpr const char* ASSET_ID_HEADER = "AssetID";
    };

    /// <summary>
    /// <para>ProjectData:</para>
    /// <para>해당 클래스는 프로젝트에 대한 Guid 및 관련 설정을 관리하는 구조체입니다.</para>
    /// <para>프로젝트의 고유한 ID를 관리하며, 프로젝트 설정 등을 관리할 수 있습니다.</para>
    /// </summary>
    class ProjectData : public FileData
    {
    private:
        virtual bool Write(YAML::Node& node) const override;
        virtual bool Read(YAML::Node& node) override;
    };

}
