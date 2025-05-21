#pragma once

namespace File
{
    namespace Interface
    {
        /*
        IFileEventProcesser:
        파일 관련 이벤트 처리용 인터페이스다.
        */
        class IFileEventProcesser
        {
        public:
            /* 엔진 내부 Context 등록 이벤트 */
            virtual void OnFileRegistered(const File::Path& path)                               = 0;
            /* 엔진 내부 Context 해제 이벤트 */
            virtual void OnFileUnregistered(const File::Path& path)                             = 0;
            /* 외부 파일 수정 이벤트 */
            virtual void OnFileModified(const File::Path& path)                                 = 0;
            /* 외부 파일 삭제 이벤트 */
            virtual void OnFileRemoved(const File::Path& path)                                  = 0;
            /* 외부 파일 이름 변경 이벤트 */
            virtual void OnFileRenamed(const File::Path& oldPath, const File::Path& newPath)    = 0;
            /* 외부 파일 이동 이벤트 */
            virtual void OnFileMoved(const File::Path& oldPath, const File::Path& newPath)      = 0;
        };
    }
}
