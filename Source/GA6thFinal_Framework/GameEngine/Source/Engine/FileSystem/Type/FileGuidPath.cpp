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

    GuidRef::GuidRef() : Guid() 
    { 
    }
    GuidRef::GuidRef(const Guid& guid) : Guid(guid)
    {
        AddRef();
    }
    GuidRef::~GuidRef()
    {
        Release();
    }
    void GuidRef::AddRef() 
    {
        UmFileSystem.AddGuidRefCount(*this);
    }
    void GuidRef::Release() 
    {
        UmFileSystem.SubGuidRefCount(*this);
    }
    std::size_t GuidRef::GetRefCount() const
    {
        return UmFileSystem.GetGuidRefCount(*this);
    }
    GuidRef& GuidRef::operator=(const File::Guid& other)
    {
        Release();
        FString::operator=(other);
        AddRef();
        return *this;
    }
    GuidRef& GuidRef::operator=(File::Guid&& other) noexcept
    {
        Release();
        FString::operator=(std::move(other));
        AddRef();
        return *this;
    }
}

