#pragma once

namespace File
{
    struct FileInformation
    {
        File::Path      FilePath;
        LONGLONG        CreationTime;
        LONGLONG        LastModificationTime;
        LONGLONG        LastChangeTime;
        LONGLONG        LastAccessTime;
        LONGLONG        FileId;
        LONGLONG        ParentFileId;
    };
}