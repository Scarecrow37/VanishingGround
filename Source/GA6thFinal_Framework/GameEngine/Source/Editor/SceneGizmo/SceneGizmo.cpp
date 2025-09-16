#include "pch.h"
#include "SceneGizmo.h"

SceneGizmo::SceneGizmo(Component* ownerComponenet) 
    : 
    _ownerComponenet(*ownerComponenet)
{
    Size = {50.f, 50.f};
    ResetOwnerMatrix();
}

void SceneGizmo::SetIconTexture(DefaultIcon icon)
{
    _icon = UmResourceManager->LoadResource<Texture>(GetIconPath(icon));
}

void SceneGizmo::SetIconTexture(const File::Path& path)
{
    UmSceneManager.ResourceManager.RequestTextureResource(&_ownerComponenet, path, 
    [this, path]() 
    { 
        _icon = UmResourceManager->LoadResource<Texture>(path);
    });
}

void SceneGizmo::DrawIcon()
{
    if constexpr (IS_EDITOR)
    {
        if (_icon)
        {
            UmGizmoManager.SubmitSceneGizmoIcon(this);
        }
    }
}

void SceneGizmo::DrawImGuizmo() 
{
    if constexpr (IS_EDITOR)
    {
        if (_ownerMatrix)
        {
            UmGizmoManager.SubminSceneImGuizmo(this);
        }
    }
}
