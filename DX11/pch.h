#pragma once


#include "Types.h"
#include "define.h"

// STL
#include <memory>
#include <memory.h>
#include <iostream>
#include <array>
#include <vector>
#include <list>
#include <map>
#include <set>
#include <unordered_map>
#include <unordered_set>
#include <algorithm>
using namespace std;

// WIN
#include <windows.h>
#include <assert.h>
#include <optional>

// DX
#include <d3d11.h>
#include <d3dcompiler.h>
#include <d3d11shader.h>
#include <d3d11.h>
#include <wrl.h>
#include <DirectXMath.h>
using namespace DirectX;
using namespace Microsoft::WRL;

// IMGUI
#include "ImGui/imgui.h"
#include "ImGui/imgui_impl_dx11.h" 
#include "ImGui/imgui_impl_win32.h" 

// LIB
#include <DirectXTex/DirectXTex.h>
#include <DirectXTex/DirectXTex.inl>
#include <FX11/d3dx11effect.h>

#pragma comment(lib, "d3d11.lib")
#pragma comment(lib, "d3dcompiler.lib")

#ifdef _DEBUG
#pragma comment(lib, "DirectXTex/DirectXTex_debug.lib")
#pragma comment(lib, "FX11/Effects11d.lib")
#pragma comment(lib, "Assimp/assimp-vc143-mtd.lib")
#else
#pragma comment(lib, "DirectXTex/DirectXTex.lib")
#pragma comment(lib, "FX11/Effects11.lib")
#pragma comment(lib, "Assimp/assimp-vc143-mt.lib")
#endif

// EXTENSIONS
#include "vector_ex.h"

// ENGINE
#include "Application.h"

#include "Shader.h"
#include "Mesh.h"
#include "GameObject.h"
#include "Transform.h"
#include "ModelRenderer.h"

#include "EditorWindow.h"
#include "EditorGUIManager.h"

#include "Scene.h"
#include "SceneManager.h"

// SHADER
#include "NormalMapSkinnedShader.h"
