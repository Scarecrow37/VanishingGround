#pragma once
#include "IGraphicsBase.h"

class Model;
class IAnimator;
class IMeshRenderer : public IGraphicsBase
{
protected:
    IMeshRenderer()          = default;
    virtual ~IMeshRenderer() = default;

public:
    virtual const UINT               GetCustomDepth(UINT meshID) const = 0;
    virtual const std::vector<UINT>& GetCustomDepths()                 = 0;
    virtual IAnimator*               GetAnimator() const               = 0;
    virtual std::shared_ptr<Model>   GetModel() const                  = 0;
    virtual const MeshType           GetType() const                   = 0;
    virtual const CustomLightType    GetCustomLightType() const        = 0;
    virtual std::vector<Material>&   GetMaterials()                    = 0;

public:
    virtual void SetMaterial(const UINT meshIndex, const Material& material)             = 0;
    virtual void SetMasterMaterial(const UINT meshIndex, const Material& material)       = 0;
    virtual void SetCustomMaterial(CustomLightType type, const std::any& customMaterial) = 0;

public:
    virtual void OnCustomDepth(UINT customDepth)               = 0;
    virtual void OnCustomDepth(UINT customDepth, UINT meshID)  = 0;
    virtual void OffCustomDepth(UINT customDepth)              = 0;
    virtual void OffCustomDepth(UINT customDepth, UINT meshID) = 0;
};