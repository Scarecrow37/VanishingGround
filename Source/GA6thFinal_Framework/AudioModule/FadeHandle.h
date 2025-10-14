#pragma once

namespace Audio
{
    /// <summary>
    /// 현재 사용중인 Effect 제어에 사용하는 FadeHandle 객체입니다.
    /// </summary>
    class FadeHandle : public EffectHandle
    {
        friend class System;

    public:
        /// <summary>
        /// 기본 생성자로 생성된 핸들은 유효하지 않습니다.
        /// </summary>
        FadeHandle();

    protected:
        FadeHandle(Index index, Generation generation);
    };
}
