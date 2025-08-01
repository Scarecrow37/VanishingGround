#include "pch.h"
#include "RenderPassDatas.h"

const std::any& RenderPassDatas::GetRenderPassProperty(std::string_view sceneName, std::string_view passName) const
{
    return _renderPassProperties.at(sceneName.data()).at(passName.data()).first;
}
