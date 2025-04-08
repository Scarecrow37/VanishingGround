#include "pch.h"
#include "MeshRenderer.h"
#include "VertexShader.h"

void MeshRenderer::SetVertexShader()
{
	_vertexShader->SetVertexShader();
}

void MeshRenderer::Release()
{
	delete this;
}

void MeshRenderer::SetDesc(const GE::MeshDescription* in)
{
	_description = *in;
}

void MeshRenderer::GetDesc(GE::MeshDescription* out)
{
	*out = _description;
}