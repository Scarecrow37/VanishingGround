#pragma once
#include "IFilter.h"

namespace GE
{
    struct BlOOM_DESC
    {
        float weight;
        float threshold;
        unsigned int level;
    };

    class IBloom : public IFilter
    {
    protected:
        explicit IBloom() = default;
        virtual ~IBloom() = default;
        IBloom(const IBloom&) = delete;
        IBloom(IBloom&&) = delete;
        IBloom& operator=(const IBloom&) = delete;
        IBloom& operator=(IBloom&&) = delete;

    public:
        virtual void GetDesc(BlOOM_DESC* pOutDesc) = 0;
        virtual void SetDesc(BlOOM_DESC* pInDesc) = 0;
    };
}