#pragma once

namespace Audio
{
    /// <summary>
    /// Audio System에서 사용되는 Handle 객체입니다.
    /// </summary>
    class Handle
    {
        friend class System;

    public:
        /// <summary>
        /// 기본 생성자로 생성된 핸들은 유효하지 않습니다.
        /// </summary>
        Handle();

    protected:
        Handle(Index index, Generation generation);

    public:
        bool operator==(const Handle& other) const noexcept;

    private:
        Index      _index;
        Generation _generation;
    };
} // namespace Audio