//-----------------------------------------------------------------------------
// File: Pch.h
//
// Desc: プリコンパイル済みヘッダー
//
// Edit: 初期配布
//-----------------------------------------------------------------------------
#pragma once

// デバッグ環境を自分で作った方がいいかも
// Project/C++/リプロセッサ/以下の項目追加
// b2 link=static runtime-link=static variant=debug runtime-debugging=on --with-filesystem --with-date_time --with-system --with-thread define=_ITERATOR_DEBUG_LEVEL=0

//--------------------------------------------------
//
// 基本
//
//--------------------------------------------------
#pragma comment(lib,"winmm.lib")

#define NOMINMAX
#include <windows.h>
#include <stdio.h>
#include <tchar.h>// _T

#include <wrl/client.h>

//--------------------------------------------------
//
// STL
//
//--------------------------------------------------
#include <map>
#include <unordered_map>
#include <unordered_set>
#include <string>
#include <array>
#include <vector>
#include <stack>
#include <list>
#include <iterator>
#include <queue>
#include <algorithm>
#include <memory>
#include <random>
#include <fstream>
#include <iostream>
#include <sstream>
#include <functional>
#include <thread>
#include <atomic>
#include <mutex>
#include <future>
#include <filesystem>

//--------------------------------------------------
//
// Direct3D11
//
//--------------------------------------------------
#pragma comment(lib,"dxguid.lib")
#pragma comment(lib,"d3d11.lib")
#pragma comment(lib,"dxgi.lib")

#include <d3dcommon.h>
#include <dxgi.h>
#include <d3d11.h>

#include <DirectXMath.h>
#include <DirectXCollision.h>

// DirectX Tool Kit
#pragma comment(lib, "DirectXTK.lib")
#pragma comment(lib, "DirectXTKAudioWin8.lib")
#include <SimpleMath.h>
#include <Audio.h>

// DirectX Tex
#pragma comment(lib, "DirectXTex.lib")
#include <DirectXTex.h>

//--------------------------------------------------
//
// ImGui
//
//--------------------------------------------------
#include "imgui/imgui.h"

#define IMGUI_DEFINE_MATH_OPERATORS
#define IMGUI_DEFINE_PLACEMENT_NEW
#include "imgui/imgui_internal.h"

#include "imgui/imgui_impl_win32.h"
#include "imgui/imgui_impl_dx11.h"
#include "imgui/imgui_stdlib.h"

//--------------------------------------------------
//
// json11
//
//--------------------------------------------------
#include "json11KdCustom/json11.hpp"

//--------------------------------------------------
// 文字コード変換
//--------------------------------------------------
#include "strconv.h"

//--------------------------------------------------
//
// Effekseer
//
//--------------------------------------------------
#pragma comment(lib, "Effekseer.lib")
#pragma comment(lib, "EffekseerRendererDX11.lib")

#include <Effekseer.h>
#include <EffekseerRendererDX11.h>

//--------------------------------------------------
//
// 自作System
//
//--------------------------------------------------
#include "System/KdSystem.h"