#pragma once

/*미니 덤프 생성 함수.*/
void CreateMiniDump(_EXCEPTION_POINTERS* pExceptionPointers, int MINIDUMP_TYPE);

/*덤프 콜백 함수.*/
__callback LONG WINAPI CustomUnhandledExceptionFilter(_In_ _EXCEPTION_POINTERS* pExceptionPointers);

