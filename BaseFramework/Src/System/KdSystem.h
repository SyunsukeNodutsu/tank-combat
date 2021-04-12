//-----------------------------------------------------------------------------
// File: KdSystem.h
//
// Desc: システム系のヘッダーファイル
//
// Edit: 初期配布
//-----------------------------------------------------------------------------
#pragma once

// 便利機能
#include "Utility/KdUtility.h"

// 算術
#include "Math/MyMath.h"
// イージング
#include "Math/Easing.h"

// バッファ
#include "Direct3D/KdBuffer.h"
// Direct3D
#include "Direct3D/KdDirect3D.h"


// テクスチャ
#include "Direct3D/KdTexture.h"
// メッシュ
#include "Direct3D/KdMesh.h"
// モデル
#include "Direct3D/KdModel.h"
// アニメーション
#include "Math/KdAnimation.h"

// 板ポリゴン
#include "Direct3D/KdSquarePolygon.h"
// 軌跡ポリゴン
#include "Direct3D/KdTrailPolygon.h"


// 判定系
#include "Math/KdCollision.h"


// シェーダ
#include "Shader/KdShaderManager.h"

// ウィンドウ
#include "Window/KdWindow.h"

// サウンド
#include "Audio/Audio.h"

// リソース管理
#include "Utility/KdResourceFactory.h"

// Effekseer
#include <EffekseerManager.h>

// 自作のボタンクラス
#include "Utility/Button.h"