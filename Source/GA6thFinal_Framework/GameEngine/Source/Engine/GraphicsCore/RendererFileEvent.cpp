#include "pch.h"
#include "RendererFileEvent.h"

void RendererFileEvent::OnFileRegistered(const File::Path& path)
{
    // 추후 에디터 실행 시 리소스를 로드 한다면 여기에 추가
}

void RendererFileEvent::OnFileModified(const File::Path& path)
{
    // 리소스 수정 시 이벤트
}

void RendererFileEvent::OnRequestedInspect(const File::Path& path)
{
    // 인스펙터에 보여줄 내용
    // ex) 미리보기
}
