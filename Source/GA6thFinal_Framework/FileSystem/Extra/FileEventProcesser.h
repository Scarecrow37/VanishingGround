#pragma once

namespace File
{
    /*
    IFileEventProcesser:
    파일 관련 이벤트 처리용 인터페이스다.
    */
    class IFileEventProcesser
    {
    public:
        virtual void OnFileAdded(const Path& path) {};
        virtual void OnFileModified(const Path& path) {};
        virtual void OnFileRemoved(const Path& path) {};
        virtual void OnFileRenamed(const Path& oldPath, const Path& newPath) {};
        virtual void OnFileMoved(const Path& oldPath, const Path& newPath) {};
    };
}
