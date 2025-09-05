#pragma once
#include "RenderPass.h"
class DistortionResolvePass : public RenderPass
{
public:
    DistortionResolvePass();
    virtual ~DistortionResolvePass();

public:


        FX<GE::VS::QUAD, GE::PS::DISTORTION_RESOLVE> _fx;
};
