#pragma once
#include "IFilter.h"

namespace GE
{
    struct BLUR_DESC
    {
    };

    class IBlur : public IFilter
    {
    protected:
        explicit IBlur() = default;
        virtual ~IBlur() = default;
        IBlur(const IBlur&) = delete;
        IBlur(IBlur&&) = delete;
        IBlur& operator=(const IBlur&) = delete;
        IBlur& operator=(IBlur&&) = delete;

    public:
        virtual void GetDesc(BLUR_DESC* pOutDesc) = 0;
        virtual void SetDesc(BLUR_DESC* pInDesc) = 0;
    };
}