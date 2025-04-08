#pragma once
#include "IBash.h"

namespace GE
{
    class IFilter : public IBase
    {
    protected:
        explicit IFilter() = default;
        virtual ~IFilter() = default;
        IFilter(const IFilter&) = delete;
        IFilter(IFilter&&) = delete;
        IFilter& operator=(const IFilter&) = delete;
        IFilter& operator=(IFilter&&) = delete;
    };
}