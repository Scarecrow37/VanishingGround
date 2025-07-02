#pragma once

namespace Audio
{
    /// <summary>
    /// 현재 사용중인 Audio 제어에 사용하는 Handle 객체입니다.
    /// </summary>
    class Handle
    {
        friend class EManager;

    public:
        /// <summary>
        /// 기본 생성자로 생성된 핸들은 유효하지 않습니다.
        /// </summary>
        Handle();

    protected:
        Handle(WaveFormatHash hash, Index index, Generation generation);

    private:
        WaveFormatHash _hash;
        Index          _index;
        Generation     _generation;
    };
} // namespace Audio