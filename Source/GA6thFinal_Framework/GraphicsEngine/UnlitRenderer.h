#pragma once
#include "includes/IUnlitRenderer.h"
#include "Component.h"

class UnlitRenderer : public Component, public GE::IUnlitRenderer
{
public:
	unsigned int GetLayer() const { return _layer; }

public:
	// IUnlitRenderer을(를) 통해 상속됨
	void SetPostEffectLayer(unsigned int layer) override;

public:
	virtual void Render() = 0;

private:
	unsigned int _layer;

};