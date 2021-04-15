//-----------------------------------------------------------------------------
// File: KdCollision.cpp
//
// Edit: 2021/04/14 野筒隼輔
//-----------------------------------------------------------------------------
#include "KdCollision.h"

//--------------------------------------------------
// using namespace DirectX: 算術系の短縮
//--------------------------------------------------
using namespace DirectX;

//-----------------------------------------------------------------------------
// Name: KdSphereToMesh()
// Desc: 球とメッシュ
//-----------------------------------------------------------------------------
bool KdSphereToMesh(const float hitRange, const KdVector3& pos, const KdMatrix& matrix)
{
	// 自分の座標ベクトル
	const KdVector3 myPos = matrix.GetTranslation();
	// 2点間のベクトルを計算
	const KdVector3 betweenVec = pos - myPos;
	// 2点間の距離を計算
	const float distance = betweenVec.Length();

	return (distance <= hitRange);
}

//-----------------------------------------------------------------------------
// Name: KdRayToMesh()
// Desc: レイとメッシュ
//-----------------------------------------------------------------------------
bool KdRayToMesh(const XMVECTOR& rRayPos, const XMVECTOR& rRayDir, float maxDistance, const KdMesh& rMesh, const KdMatrix& rMatrix, KdRayResult& rResult)
{
	// モデルの逆行列でレイを変換
	XMMATRIX invMat = XMMatrixInverse(0, rMatrix);

	// レイの判定開始位置を逆反転
	XMVECTOR rayPos = XMVector3TransformCoord(rRayPos, invMat);

	// 発射方向は正規化されてないと正しく判定できないので正規化
	XMVECTOR rayDir = XMVector3TransformNormal(rRayDir, invMat);

	// 逆行列に拡縮が入っていると
	// レイが当たった距離にも拡縮が反映されてしまうので
	// 判定用の最大距離にも拡縮を反映させておく
	float dirLength = XMVector3Length(rayDir).m128_f32[0];
	float rayChackRange = maxDistance * dirLength;
	rayDir = XMVector3Normalize(rayDir);

	//--------------------------------------------------
	// ブロードフェイズ
	// AABBvsレイ
	//--------------------------------------------------
	{
		// AABB vs レイ
		float AABBdist = FLT_MAX;
		if (!rMesh.GetBoundingBox().Intersects(rayPos, rayDir, AABBdist)) { return false; }

		// 最大距離なら範囲外なので中止
		if (AABBdist > rayChackRange) { return false; }
	}

	//--------------------------------------------------
	// ナローフェイズ
	// レイvsすべての面
	//--------------------------------------------------

	// 当たったかどうか
	bool result = false;
	// 当たった面との距離
	float closestDist = FLT_MAX;

	// 面情報の所得
	const KdMeshFace* pFace = &rMesh.GetFaces()[0];
	const UINT faceNum = rMesh.GetFaces().size();

	// すべての面(三角形)と当たり判定
	for (UINT faceIdx = 0; faceIdx < faceNum; ++faceIdx) {
		// 三角形を構成する3つの頂点のIndex
		const UINT* idx = pFace[faceIdx].Idx;

		// レイと三角形の当たり判定
		float triDist = FLT_MAX;
		const bool isHit = TriangleTests::Intersects(
			rayPos,								// 発射場所
			rayDir,								// 発射方向(正規化済み)
			rMesh.GetVertexPositions()[idx[0]],	// 判定する3角形の頂点情報
			rMesh.GetVertexPositions()[idx[1]],
			rMesh.GetVertexPositions()[idx[2]],
			triDist								// 当たった場合の距離
		);

		// ヒットしていなかったらスキップ
		if (!isHit) { continue; }

		// 最大距離内か
		if (triDist <= rayChackRange) {
			// 当たったとする
			result = true;
			// 当たり判定でとれる距離は拡縮に影響がないので、実際の長さを計算する
			triDist /= dirLength;

			// 近いほうを 距離を更新
			if (triDist < closestDist) {
				closestDist = triDist;
				rResult.m_indexNum = faceIdx;
			}
		}
	}

	// 結果情報設定
	rResult.m_distance = closestDist;
	rResult.m_hit = result;
	if (rResult.m_hit) {
		rResult.m_hitPos = rRayPos + rayDir * closestDist;
	}

	return result;
}

//-----------------------------------------------------------------------------
// Name: KdSphereToMesh()
// Desc: 
//-----------------------------------------------------------------------------
bool KdSphereToMesh(const Math::Vector3& aSpherePos, float aRadius, const KdMesh& aMesh, const XMMATRIX& aMatrix, Math::Vector3& aPushedPos)
{
	//--------------------------------------------------
	// ブロードフェイズ
	// AABB(BoundingBox) vs BS(BoundingSphere)
	//--------------------------------------------------
	{
		// メッシュのAABBを元に、行列で変換したAABBを作成
		BoundingBox aabb = {};
		aMesh.GetBoundingBox().Transform(aabb, aMatrix);

		// 判定した結果衝突していなければ以降の処理はしない
		BoundingSphere bs(aSpherePos, aRadius);
		if (!aabb.Intersects(bs)) { return false; }
	}

	//--------------------------------------------------
	// ナローフェイズ
	// 球 vs メッシュ
	//--------------------------------------------------

	// 衝突したかどうか
	bool isHit = false;

	// DEBUGビルドでも速度を維持するために、別変数に拾っていく
	const auto* pFaces = &aMesh.GetFaces()[0];
	UINT faceNum = aMesh.GetFaces().size();
	const Math::Vector3* vertices = &aMesh.GetVertexPositions()[0];

	// メッシュの逆行列で球の中心座標を変換 ※メッシュの座標系へ変換される
	XMMATRIX invMat = XMMatrixInverse(0, aMatrix);
	XMVECTOR QPos = aSpherePos;
	QPos = XMVector3TransformCoord(QPos, invMat);

	// 半径の二乗を計算しておく ※高速化処理
	float radiusSq = aRadius * aRadius;

	// 行列の各軸の拡大率を計算しておく
	XMVECTOR scale = {};
	scale.m128_f32[0] = XMVector3Length(aMatrix.r[0]).m128_f32[0];
	scale.m128_f32[1] = XMVector3Length(aMatrix.r[1]).m128_f32[0];
	scale.m128_f32[2] = XMVector3Length(aMatrix.r[2]).m128_f32[0];
	scale.m128_f32[3] = 0;

	// 計算用変数
	XMVECTOR nearPt = {};// 最近接点
	XMVECTOR vToCenter = {};

	// 全ての面と判定 ※判定はメッシュのローカル空間で行われる
	for (UINT i = 0; i < faceNum; i++) {
		// 点と三角形の最近接点を算出
		KdPointToTriangle(QPos, vertices[pFaces[i].Idx[0]], vertices[pFaces[i].Idx[1]], vertices[pFaces[i].Idx[2]], nearPt);
		// 最近接点から球の中心へのベクトルを算出
		vToCenter = QPos - nearPt;
		// 拡大率を加味
		vToCenter *= scale;

		// vToCenterが半径以内の場合は衝突している
		if (XMVector3LengthSq(vToCenter).m128_f32[0] <= radiusSq) {
			// 最近接点を出力データに記憶する ※逆行列に変換したものを元に返す
			nearPt = XMVector3TransformCoord(nearPt, aMatrix);

			// 押し戻し処理
			XMVECTOR v = XMVector3Normalize(vToCenter);
			v *= aRadius - XMVector3Length(vToCenter).m128_f32[0];
			// 拡縮を考慮した座標系へ戻す
			v /= scale;
			// 球の座標を移動
			QPos += v;

			// 衝突結果ON
			isHit = true;
		}
	}

	// 衝突していた場合押し戻された球の座標を格納
	if (isHit) { aPushedPos = XMVector3TransformCoord(QPos, aMatrix); }

	return isHit;
}

//-----------------------------------------------------------------------------
// Name: KdPointToTriangle()
// Desc: 
//-----------------------------------------------------------------------------
void KdPointToTriangle(const XMVECTOR& p, const XMVECTOR& a, const XMVECTOR& b, const XMVECTOR& c, XMVECTOR& outPt)
{
	// ※参考:[書籍]「ゲームプログラミングのためのリアルタイム衝突判定」

	// pがaの外側の頂点領域の中にあるかどうかチェック
	XMVECTOR ab = b - a;
	XMVECTOR ac = c - a;
	XMVECTOR ap = p - a;

	float d1 = XMVector3Dot(ab, ap).m128_f32[0];//ab.Dot(ap);
	float d2 = XMVector3Dot(ac, ap).m128_f32[0];//ac.Dot(ap);
	if (d1 <= 0.0f && d2 <= 0.0f) {
		outPt = a; // 重心座標(1,0,0)
		return;
	}

	// pがbの外側の頂点領域の中にあるかどうかチェック
	XMVECTOR bp = p - b;
	float d3 = XMVector3Dot(ab, bp).m128_f32[0];//ab.Dot(bp);
	float d4 = XMVector3Dot(ac, bp).m128_f32[0];//ac.Dot(bp);
	if (d3 >= 0.0f && d4 <= d3) {
		outPt = b; // 重心座標(0,1,0)
		return;
	}


	// pがabの辺領域の中にあるかどうかをチェックし、あればpのab上に対する射影を返す
	float vc = d1 * d4 - d3 * d2;
	if (vc <= 0.0f && d1 >= 0.0f && d3 <= 0.0f) {
		float v = d1 / (d1 - d3);
		outPt = a + ab * v; // 重心座標(1-v,v,0)
		return;
	}

	// pがcの外側の頂点領域の中にあるかどうかチェック
	XMVECTOR cp = p - c;
	float d5 = XMVector3Dot(ab, cp).m128_f32[0];//ab.Dot(cp);
	float d6 = XMVector3Dot(ac, cp).m128_f32[0];//ac.Dot(cp);
	if (d6 >= 0.0f && d5 <= d6) {
		outPt = c; // 重心座標(0,0,1)
		return;
	}

	// pがacの辺領域の中にあるかどうかをチェックし、あればpのac上に対する射影を返す
	float vb = d5 * d2 - d1 * d6;
	if (vb <= 0.0f && d2 >= 0.0f && d6 <= 0.0f) {
		float w = d2 / (d2 - d6);
		outPt = a + ac * w; // 重心座標(1-w,0,w)
		return;
	}

	// pがbcの辺領域の中にあるかどうかをチェックし、あればpのbc上に対する射影を返す
	float va = d3 * d6 - d5 * d4;
	if (va <= 0.0f && (d4 - d3) >= 0.0f && (d5 - d6) >= 0.0f) {
		float w = (d4 - d3) / (d4 - d3) + (d5 - d6);
		outPt = b + (c - b) * w; // 重心座標(0,1-w,w)
		return;
	}

	// pは面領域の中にある。Qをその重心座標(u,v,w)を用いて計算
	float denom = 1.0f / (va + vb + vc);
	float v = vb * denom;
	float w = vc * denom;
	outPt = a + ab * v + ac * w; // = u*a + v*b + w*c, u = va*denom = 1.0f - v - w
}