//-----------------------------------------------------------------------------
// File: KdCollision.h
//
// Edit: 2021/03/18 野筒隼輔
//-----------------------------------------------------------------------------
#pragma once

//--------------------------------------------------
// Name: struct KdRayResult
// Desc: レイ判定をした際の結果情報
//--------------------------------------------------
struct KdRayResult
{
	float		m_distance	= FLT_MAX;	// 衝突したところまでの距離
	bool		m_hit		= false;	// 衝突したかどうか
	KdVector3	m_hitPos	= {};		// 衝突した座標
	UINT		m_indexNum	= 0;		// 面の番号
};

//--------------------------------------------------
// Name: KdSphereToMesh()
// Desc: 球の衝突判定
// Ret : 衝突...true 回避...false
//--------------------------------------------------
bool KdSphereToSphere(
	const float aHitRange,
	const KdVector3& aPos,
	const KdMatrix& aMatrix
);

//--------------------------------------------------
// Name: KdRayToMesh()
// Desc: レイの衝突判定
// Ret : 衝突...true 回避...false
//--------------------------------------------------
bool KdRayToMesh(
	const DirectX::XMVECTOR& rRayPos,	//
	const DirectX::XMVECTOR& rRayDir,	//
	float maxDistance,					//
	const KdMesh& rMesh,				//
	const KdMatrix& rMatrix,			//
	KdRayResult& rResult				// 結果の情報を格納
);

//--------------------------------------------------
// Name: KdSphereToMesh()
// Desc: 球とメッシュによる当たり判定
// Ret : 衝突...true 回避...false
//--------------------------------------------------
bool KdSphereToMesh(
	const Math::Vector3& aSpherePos,	// 球の中心点
	float aRadius,						// 球の半径
	const KdMesh& aMesh,				// 判定するメッシュ情報
	const DirectX::XMMATRIX& aMatrix,	// 判定する相手の行列
	Math::Vector3& aPushedPos			// 当たっていた場合、押し返された球の中心点
);

//--------------------------------------------------
// Name: KdPointToTriangle()
// Desc: 点と三角形の最近接点を算出
//--------------------------------------------------
void KdPointToTriangle(
	const DirectX::XMVECTOR& aPoint,	// 点の座標
	const DirectX::XMVECTOR& aP1,		// 三角形の点1
	const DirectX::XMVECTOR& aP2,		// 三角形の点2
	const DirectX::XMVECTOR& aP3,		// 三角形の点3
	DirectX::XMVECTOR& aOutPt			// 最近接点の座標が入る
);