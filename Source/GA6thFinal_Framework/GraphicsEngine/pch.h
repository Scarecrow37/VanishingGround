#ifndef PCH_H
#define PCH_H

// 여기에 미리 컴파일하려는 헤더 추가
#define NOMINMAX
#include <algorithm>
#include <array>
#include <filesystem>
#include <fstream>
#include <memory>
#include <queue>
#include <random>
#include <stack>
#include <string>
#include <type_traits>
#include <typeindex>
#include <unordered_map>
#include <map>
#include <unordered_set>
#include <variant>
#include <vector>
#include <windows.h>
#include <wrl.h>
#include <set>
#include <any>
#include <format>
#include <concurrent_queue.h>
#include <functional>
#include <ranges>

#include "directx/d3dcommon.h"
#include "directx/d3dx12.h"
#include "directxtk12/DebugDraw.h"
#include "directxtk12/ResourceUploadBatch.h"
#include "directxtk12/SimpleMath.h"
#include "directxtk12/SpriteBatch.h"
#include "directxtk12/SpriteFont.h"
#include "DirectXTex.h"
#include "yyjson/yyjson.h"

#include <d3dcompiler.h>
#include <dbghelp.h>
#include <dxcapi.h>
#include <dxgi1_4.h>
#include <dxgi1_6.h>

#ifdef _DEBUG
#pragma comment(lib, "DirectXTex_d.lib")
#pragma comment(lib, "DirectXTK12_d.lib")
#else
#pragma comment(lib, "DirectXTex.lib")
#pragma comment(lib, "DirectXTK12.lib")
#endif

#pragma comment(lib, "assimp-vc143-mt.lib")
#pragma comment(lib, "d3d12")
#pragma comment(lib, "dxgi")
#pragma comment(lib, "Dbghelp.lib")
#pragma comment(lib, "d3dcompiler")
#pragma comment(lib, "dxcompiler")

using namespace DirectX;
using namespace DirectX::SimpleMath;
using namespace Microsoft::WRL;

#include "assimp/Importer.hpp"
#include "assimp/postprocess.h"
#include "assimp/scene.h"

#include "ShaderTypes.generated.h"
#include "Defines.h"
#include "Enums.h"
#include "Structs.h"
#include "Utility.h"
#include "Headers.h"
#include "Externs.h"

#include "GraphicsCore.h"

#endif //PCH_H