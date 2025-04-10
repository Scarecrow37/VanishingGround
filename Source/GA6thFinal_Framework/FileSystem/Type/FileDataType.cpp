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
        return IDMapper::GetPathFromGuid(native());
    }
    Path::operator File::Guid() const
    {
        return ToGuid();
    }
    File::Guid Path::ToGuid() const
    {
        return IDMapper::GetGuidFromPath(native());
    }
    bool MetaData::Create(const Path& path)
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
            CreateGuid(_projectGuid);
            YAML::Node node;
            node[FILE_GUID_HEADER] = _fileGuid.string();
            node[PROJ_GUID_HEADER] = _projectGuid.string();
            fout << node;
            return true;
        }
        return false;
    }
    bool MetaData::Load(const Path& path)
    {
        if (true == path.empty())
            return false;

        if (false == stdfs::exists(path))
            return false;

        YAML::Node node = YAML::LoadFile(path.string());
        if (true == node.IsNull())
            return false;

        if (node[FILE_GUID_HEADER] && node[PROJ_GUID_HEADER])
        {
            _filePath    = path;
            _fileGuid    = node[FILE_GUID_HEADER].as<std::string>();
            _projectGuid = node[PROJ_GUID_HEADER].as<std::string>();
            return true;
        }   
        return false;
    }
    bool MetaData::Move(const Path& path)
    {
        if (true == stdfs::exists(_filePath))
        {
            stdfs::rename(_filePath, path);
            _filePath = path;
            return true;
        }
        return false;
    }
    bool MetaData::Remove()
    {
        if (true == stdfs::exists(_filePath))
        {
            stdfs::remove(_filePath);
            _filePath    = "";
            _fileGuid    = NULL_GUID;
            _projectGuid = NULL_GUID;
            return true;
        }
        return false;
    }
    bool MetaData::IsNull()
    {
        return _fileGuid == NULL_GUID;
    }
}

