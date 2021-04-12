//-----------------------------------------------------------------------------
// File: CollisionComponent.h
//
// Edit: 2021/03/21 野筒隼輔
//-----------------------------------------------------------------------------
#include "CollisionComponent.h"
#include "ModelComponent.h"

//-----------------------------------------------------------------------------
// Name: CollisionComponent()
// Desc: コンストラクタ
//-----------------------------------------------------------------------------
CollisionComponent::CollisionComponent(GameObject& owner)
	: m_owner(owner)
{
}

//-----------------------------------------------------------------------------
// Name: HitCheckBySphere()
// Desc: 球判定 ※修正.半径の管理
//-----------------------------------------------------------------------------
const bool CollisionComponent::HitCheckBySphere(const SphereInfo& info)
{
	if (!m_owner.GetModelComponent()) { return false; }

	// 当たったとする距離の計算(お互いの半径を加算)
	const float hitRange = info.m_radius + 1.0f;// m_owner.GetColRad();

	return KdSphereToMesh(hitRange, info.m_pos, m_owner.GetMatrix());
}

//-----------------------------------------------------------------------------
// Name: HitCheckByRay()
// Desc: レイ判定
//-----------------------------------------------------------------------------
const bool CollisionComponent::HitCheckByRay(const RayInfo& info, KdRayResult& result)
{
	if (!m_owner.GetModelComponent()) { return false; }

	for (auto& node : m_owner.GetCollModelComponent()->GetChangeableNodes())
	{
		if (!node.m_spMesh) { continue; }

		// レイ判定
		KdRayResult tmpResult;
		KdRayToMesh(info.m_pos, info.m_dir, info.m_maxRange, *(node.m_spMesh), node.m_localTransform * m_owner.GetMatrix(), tmpResult);

		// より近い判定を優先する(複数のNode(パーツ)から)
		if (tmpResult.m_distance < result.m_distance) { result = tmpResult; }
	}

	return result.m_hit;
}

//-----------------------------------------------------------------------------
// Name: HitCheckBySphereVsMesh()
// Desc: 球とメッシュ判定
//-----------------------------------------------------------------------------
const bool CollisionComponent::HitCheckBySphereVsMesh(SphereInfo& info, SphereResult& result)
{
	if (!m_owner.GetModelComponent()) { return false; }

	// 全てのノードのメッシュから押し返された位置を格納
	KdVector3 pushedFromNodesPos = info.m_pos;

	for (auto& node : m_owner.GetCollModelComponent()->GetChangeableNodes())
	{
		if (!node.m_spMesh) { continue; }

		// 点とノードの判定
		if (KdSphereToMesh(pushedFromNodesPos, info.m_radius, *node.m_spMesh, node.m_localTransform * m_owner.GetMatrix(), pushedFromNodesPos)) {
			result.m_isHit = true;
		}
	}

	if (result.m_isHit) {
		// 押し戻す分のベクトルを計算
		result.m_push = pushedFromNodesPos - info.m_pos;
	}

	return result.m_isHit;
}