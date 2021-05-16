//-----------------------------------------------------------------------------
// File: CollisionComponent.h
//
// Edit: 2021/03/21 野筒隼輔
//-----------------------------------------------------------------------------
#pragma once
#include "Application/Game/GameObject.h"

//--------------------------------------------------------
// 前方宣言
//--------------------------------------------------------
struct SphereInfo;
struct RayInfo;
struct SphereResult;

//-----------------------------------------------------------------------------
// Name: class CollisionComponent
// Desc: 衝突判定 部品化
//-----------------------------------------------------------------------------
class CollisionComponent
{
public:

	// コンストラクタ
	CollisionComponent(GameObject& owner);
	// 球判定
	const bool HitCheckBySphere(const SphereInfo& info);
	// レイ判定
	const bool HitCheckByRay(const RayInfo& info, KdRayResult& result);
	// 球とメッシュ判定
	const bool HitCheckBySphereVsMesh(SphereInfo& info, SphereResult& result);

private:
	GameObject& m_owner;	// オーナー
};

//--------------------------------------------------------
// Name: struct SphereInfo
// Desc: 球判定用データ
//--------------------------------------------------------
struct SphereInfo
{
	KdVector3	m_pos = KdVector3(0.0f, 0.0f, 0.0f);
	float		m_radius = 0.0f;
};

//--------------------------------------------------------
// Name: struct RayInfo
// Desc: レイ判定用データ
//--------------------------------------------------------
struct RayInfo
{
	KdVector3	m_pos = KdVector3(0.0f, 0.0f, 0.0f);
	KdVector3	m_dir = KdVector3(0.0f, 0.0f, 0.0f);
	float		m_maxRange = FLT_MAX;
};

//--------------------------------------------------------
// Name: struct SphereResult
// Desc: 球判定の結果データ
//--------------------------------------------------------
struct SphereResult
{
	KdVector3	m_push = KdVector3(0.0f, 0.0f, 0.0f);// 押し戻すベクトル
	bool		m_isHit = false;
};