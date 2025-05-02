#include "pch.h"
#include "FileDataType.h"

namespace File
{
    Guid::operator File::Path() const
    {
        return ToPath();
    }
    File::Path Guid::ToPath() const
    {
        return UmFileSystem.GetPathFromGuid(native());
    }
    bool Guid::IsNull() const
    {
        return NULL_GUID == native();
    }
    Path::operator File::Guid() const
    {
        return ToGuid();
    }
    File::Path Path::operator+(const File::FString& v)
    {
        return string() + v.string();
    }
    File::Path Path::operator/(const File::FString& v)
    {
        File::Path path = string();
        path /= v;
        return path.generic_string();
    }
    File::Guid Path::ToGuid() const
    {
        return UmFileSystem.GetGuidFromPath(native());
    }
    bool Path::IsNull() const
    {
        return NULL_PATH == native();
    }
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
            else
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

        if (true == path.empty())
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
}

