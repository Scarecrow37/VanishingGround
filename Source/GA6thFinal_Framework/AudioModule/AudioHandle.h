#pragma once
#include "Handle.h"

namespace Audio
{
    /// <summary>
    /// 현재 사용중인 Audio 제어에 사용하는 AudioHandle 객체입니다.
    /// </summary>
    class AudioHandle : public Handle
    {
        friend class System;

    public:
        /// <summary>
        /// 기본 생성자로 생성된 핸들은 유효하지 않습니다.
        /// </summary>
        AudioHandle();

    protected:
        AudioHandle(WaveFormatHash hash, Index index, Generation generation);

    private:
        WaveFormatHash _hash;
    };
} // namespace Audio