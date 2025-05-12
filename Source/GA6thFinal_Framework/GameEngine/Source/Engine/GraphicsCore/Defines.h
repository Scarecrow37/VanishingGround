#pragma once

#define ERROR_MESSAGE(msg) MessageBox(NULL, msg, L"Error", MB_OK | MB_ICONERROR)
#define FAILED_CHECK_BREAK(hr) if (FAILED(hr)) __debugbreak()

#ifdef _DEBUG
#define ASSERT(expression, msg) \
if(!expression) { ERROR_MESSAGE(msg); assert(false); }
#else
#define ASSERT(expression, msg) ((void)0)
#endif