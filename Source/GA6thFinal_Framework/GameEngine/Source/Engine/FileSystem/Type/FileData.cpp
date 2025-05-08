#include "pch.h"
#include "FileData.h"

namespace File
{
    FileData::FileData(File::Path path) : _filePath(path), _fileGuid(path) {}
    FileData::FileData(File::Guid guid) : _filePath(guid), _fileGuid(guid) {}

    bool FileData::FileCreate(bool isHidden) const
    {
        std::ofstream fout(_filePath);
        if (false == fout.is_open())
        {
            OutputLog(L"File Open Error");
            return false;
        }
        else
        {
            // 숨김 설정
            if (isHidden)
            {
                SetFileAttributesW(_filePath.c_str(), FILE_ATTRIBUTE_HIDDEN);
            }

            YAML::Node node;
            node[FILE_GUID_HEADER] = _fileGuid.string();
            if (false == Write(node))
            {
                return false;
            }
            fout << node;
            fout.close();

            return true;
        }

        return false;
    }

    bool FileData::FileRemove() const
    {
        if (true == fs::exists(_filePath))
        {
            fs::remove(_filePath);
            return true;
        }
    }

    bool FileData::Create(const File::Path& path, bool isEmpty, bool isHidden)
    {
        if (path != _filePath)
        {
            _filePath = path;
        }
        if (true == IsNull())
        {
            if (true == isEmpty)
            {
                _fileGuid = NULL_GUID;
            }
            else if (true == _fileGuid.IsNull())
            {
                CreateGuid(_fileGuid);
            }
        }

        bool result = FileCreate(isHidden);

        if (false == result)
        {
            _filePath = NULL_PATH;
            _fileGuid = NULL_GUID;
            return false;
        }
        return true;
    }

    bool FileData::Load(const Path& path)
    {
        _fileGuid = NULL_GUID;
        _filePath = NULL_PATH;

        if (true == path.IsNull())
            return false;

        if (false == fs::exists(path))
            return false;

        YAML::Node node = YAML::LoadFile(path.string());
        if (true == node.IsNull())
            return false;

        _filePath = path;

        if (node[FILE_GUID_HEADER])
        {
            _fileGuid = node[FILE_GUID_HEADER].as<std::string>();

            if (false == Read(node))
            {
                return false;
            }

            if (UmFileSystem.GetDebugLevel() >= 2)
                OutputLog(L"Load MetaFile: " + _filePath.wstring());

            return true;
        }
        return false;
    }

    bool FileData::Move(const Path& path)
    {
        if (true == fs::exists(_filePath))
        {
            fs::rename(_filePath, path);
            _filePath = path;
            return true;
        }
        return false;
    }

    bool FileData::Clear()
    {
        if (true == fs::exists(_filePath))
        {
            _filePath = NULL_PATH;
            _fileGuid = NULL_GUID;
            return true;
        }
        return false;
    }

    bool FileData::IsNull() const
    {
        return _fileGuid == NULL_GUID;
    }
    /// <summary>
    ///
    /// </summary>
    /// <param name="node"></param>
    /// <returns></returns>
    bool MetaData::Write(YAML::Node& node) const
    {
        return true;
    }

    bool MetaData::Read(YAML::Node& node) const
    {
        return true;
    }
    /// <summary>
    ///
    /// </summary>
    /// <param name="node"></param>
    /// <returns></returns>
    bool ProjectData::Write(YAML::Node& node) const
    {
        return true;
    }

    bool ProjectData::Read(YAML::Node& node) const
    {
        return true;
    }
} // namespace File