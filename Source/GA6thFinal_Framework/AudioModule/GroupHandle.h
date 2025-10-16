#pragma once
#include "Handle.h"

namespace Audio
{
    /// <summary>
    /// 현재 사용중인 Group 제어에 사용하는 GroupHandle 객체입니다.
    /// </summary>
    class GroupHandle : public Handle
    {
        friend class System;

    public:
        /// <summary>
        /// 기본 생성자로 생성된 핸들은 유효하지 않습니다.
        /// </summary>
        GroupHandle();

    protected:
        GroupHandle(Index index, Generation generation);

    public:
        bool operator==(const GroupHandle& other) const noexcept;

    };
} // namespace Audio