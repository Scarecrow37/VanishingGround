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

    bool FileData::Create(const File::Path& path, bool isHidden)
    {
        std::ofstream fout(path);
        if (false == fout.is_open())
        {
            OutputLog(L"File Open Error");
            return false;
        }
        else
        {
            _filePath = path;
            CreateGuid(_fileGuid);

            // 숨김 설정
            if (isHidden)
            {
                SetFileAttributesW(path.c_str(), FILE_ATTRIBUTE_HIDDEN);
            }
           
            YAML::Node node;
            {
                node[FILE_GUID_HEADER] = _fileGuid.string();

                if (false == Write(node))
                {
                    return false;
                }
            }

            fout << node;
            fout.close();

            return true;
        }
        return false;
    }

    bool FileData::Load(const Path& path)
    {
        _fileGuid = NULL_GUID;
        _filePath = NULL_PATH;

        if (true == path.empty())
            return false;

        if (false == stdfs::exists(path))
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
        if (true == stdfs::exists(_filePath))
        {
            stdfs::rename(_filePath, path);
            _filePath = path;
            return true;
        }
        return false;
    }

    bool FileData::Remove()
    {
        if (true == stdfs::exists(_filePath))
        {
            stdfs::remove(_filePath);
            _filePath = "";
            _fileGuid = NULL_GUID;
            return true;
        }
        return false;
    }

    bool FileData::IsNull()
    {
        return _fileGuid == NULL_GUID;
    }
    /// <summary>
    /// 
    /// </summary>
    /// <param name="node"></param>
    /// <returns></returns>
    bool MetaData::Write(YAML::Node& node)
    {
        return true;
    }

    bool MetaData::Read(YAML::Node& node)
    {
        return true;
    }
    /// <summary>
    /// 
    /// </summary>
    /// <param name="node"></param>
    /// <returns></returns>
    bool ProjectData::Write(YAML::Node& node)
    {
        return true;
    }

    bool ProjectData::Read(YAML::Node& node)
    {
        return true;
    }
}

