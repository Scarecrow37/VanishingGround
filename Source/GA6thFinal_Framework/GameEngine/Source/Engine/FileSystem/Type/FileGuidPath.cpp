#include "pch.h"
#include "FileGuidPath.h"

namespace File
{
    Guid::operator const File::Path&() const
    {
        return ToPath();
    }
    const File::Path& Guid::ToPath() const
    {
        return UmFileSystem.GetPathFromGuid(native());
    }
    bool Guid::IsNull() const
    {
        return NULL_GUID == native();
    }
    Path::operator const File::Guid&() const
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
    const File::Guid& Path::ToGuid() const
    {
        return UmFileSystem.GetGuidFromPath(native());
    }
    bool Path::IsNull() const
    {
        return NULL_PATH == native();
    }
}

