#include "pch.h"
#include "SceneGizmo.h"

SceneGizmo::SceneGizmo(Component* owner) 
: 
    _owner(*owner) 
{
    Size = {50.f, 50.f};
}

void SceneGizmo::SetIconTexture(DefaultIcon icon)
{
    _icon = UmResourceManager->LoadResource<Texture>(GetIconPath(icon));
}

void SceneGizmo::SetIconTexture(const File::Path& path)
{
    UmSceneManager.ResourceManager.RequestTextureResource(&_owner, path, 
    [this, path]() 
    { 
        _icon = UmResourceManager->LoadResource<Texture>(path);
    });
}

void SceneGizmo::Draw() 
{
    if constexpr (IS_EDITOR)
    {
        if (_icon)
        {
            UmGizmoManager.SubmitSceneGizmo(this);
        }
    }
}
