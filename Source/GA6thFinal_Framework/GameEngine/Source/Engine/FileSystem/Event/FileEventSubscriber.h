#pragma once

namespace File
{
    /// <summary>
    /// <para>FileEventSubscriber:</para>
    /// <para>해당 클래스는 File에 대한 이벤트를 받기 위해 상속하는 클래스입니다.</para>
    /// <para>처리해야 할 각 이벤트 함수를 오버라이딩하여 구현하면 됩니다.</para>
    /// </summary>
    class FileEventSubscriber abstract
        : public Interface::IFileEventProcesser
    {
        friend class EFileSystem;
    public:
        FileEventSubscriber();
        virtual ~FileEventSubscriber();

    public:
        /// <summary>FileContext가 등록된 후에 호출됩니다.</summary>
        virtual void OnFileRegistered(const File::Path& path) override                  {}
        /// <summary>FileContext가 등록 해제되기 전에 호출됩니다.</summary>
        virtual void OnFileUnregistered(const File::Path& path) override                {}
        /// <summary>File이 수정되었을 때 호출됩니다.</summary>
        virtual void OnFileModified(const Path& path) override                          {}
        /// <summary>File이 삭제되었을 때 호출됩니다.</summary>
        virtual void OnFileRemoved(const Path& path) override                           {}
        /// <summary>File이 리네이밍되었을 때 호출됩니다.</summary>
        virtual void OnFileRenamed(const Path& oldPath, const Path& newPath) override   {}
        /// <summary>File이 이동되었을 때 호출됩니다.</summary>
        virtual void OnFileMoved(const Path& oldPath, const Path& newPath) override     {}
        
        /// <summary>프로젝트 세이브를 하기 전에 처리할 동작을 구현 (확장자가 상관 없는 공통 이벤트).</summary>
        virtual void OnRequestedSave()                                                  {}
        /// <summary>프로젝트 세이브를 한 후에 처리할 동작을 구현 (확장자가 상관 없는 공통 이벤트).</summary>
        virtual void OnPostRequestedSave()                                              {}
        /// <summary>프로젝트 로드를 하기 전에 처리할 동작을 구현 (확장자가 상관 없는 공통 이벤트).</summary>
        virtual void OnRequestedLoad()                                                  {}
        /// <summary>프로젝트 로드를 한 후에 처리할 동작을 구현 (확장자가 상관 없는 공통 이벤트).</summary>
        virtual void OnPostRequestedLoad()                                              {}
        /// <summary>인스펙터에 출력 요청을 처리할 동작을 구현</summary>
        virtual void OnRequestedInspect(const File::Path& path)                         {}
        /// <summary>파일을 여는 요청을 처리할 동작을 구현</summary>
        virtual void OnRequestedOpen(const File::Path& path)                            {}
        /// <summary>파일을 복사했을 때 처리할 동작을 구현</summary>
        virtual void OnRequestedCopy(const File::Path& path)                            {}
        /// <summary>파일을 붙여넣었을 때 처리할 동작을 구현</summary>
        virtual void OnRequestedPaste(const File::Path& path)                           {}
        /// <summary>파일을 드래그 드랍했을 때 처리할 동작을 구현</summary>
        virtual void OnRequestedDragDrop(const File::Path& path)                        {}

    public:
        /// <summary>
        /// 해당 확장자가 대응되는 확장자인지 확인합니다.
        /// </summary>
        /// <param name="ext">확인할 확장자</param>
        /// <returns>대응되는지에 대한 여부</returns>
        bool IsTriggerExtension(const File::FString& ext);

        /// <summary>
        /// 대응되는 확장자 목록을 반환합니다.
        /// </summary>
        /// <returns>대응되는 확장자 목록</returns>
        std::vector<FString> GetTriggerExtensions() const;

    private:
        std::unordered_set<FString> _triggerExtTable;
    };
}

