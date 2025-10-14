#pragma once

#include "ThirdParty/include/directx/d3dx12.h"
#include "ThirdParty/include/directxtk12/SimpleMath.h"
#include "ThirdParty/include/directxtk12/SpriteBatch.h"
#include "ThirdParty/include/directxtk12/SpriteFont.h"

#pragma comment(lib, "d3d12")
#pragma comment(lib, "dxgi")
#pragma comment(lib, "Dbghelp.lib")
#pragma comment(lib, "d3dcompiler")
#pragma comment(lib, "assimp-vc143-mt.lib")

#ifdef _DEBUG
#pragma comment(lib, "DirectXTex_d.lib")
#pragma comment(lib, "DirectXTK12_d.lib")
#else
#pragma comment(lib, "DirectXTex.lib")
#pragma comment(lib, "DirectXTK12.lib")
#endif

using namespace DirectX;
using namespace DirectX::SimpleMath;

#include "Graphics_Enums.h"
#include "Graphics_Structs.h"
#include "GraphicsPointer.h"
#include "SharedResource.h"

#include "Texture.h"
#include "Model.h"
#include "Camera.h"
#include "Animation.h"
#include "Font.h"
#include "BaseMesh.h"

#include "CommandSet.h"

#include "ParticleEmitter.h"
#include "ParticleEffect.h"
#include "SceneTransitionCore.h"

#include "ParticleManager.h"
#include "ResourceManager.h"

#include "RenderPassDatas.h"

#include "GraphicsCore.h"
