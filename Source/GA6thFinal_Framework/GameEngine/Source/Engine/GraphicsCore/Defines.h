#pragma once

#define ERROR_MESSAGE(msg) MessageBox(NULL, msg, L"Error", MB_OK | MB_ICONERROR)
// #define FAILED_CHECK_BREAK(hr) if (FAILED(hr)) __debugbreak()
#define FAILED_CHECK_MESSAGE(hr, msg)                                                                                  \
    if (FAILED(hr))                                                                                                    \
    {                                                                                                                  \
        ERROR_MESSAGE(msg);                                                                                            \
        __debugbreak();                                                                                                \
    }

#ifdef _DEBUG
#define GRAPHICS_ASSERT(expression, msg)                                                                               \
    if (!(expression))                                                                                                 \
    {                                                                                                                  \
        ERROR_MESSAGE(msg);                                                                                            \
        assert(false);                                                                                                 \
    }
#else
#define GRAPHICS_ASSERT(expression, msg) ((void)0)
#endif