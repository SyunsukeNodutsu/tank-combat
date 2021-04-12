//-----------------------------------------------------------------------------
// File: KdTrailPolygon.cpp
//
// Edit: 2021/03/07 野筒隼輔
//-----------------------------------------------------------------------------
#include "KdTrailPolygon.h"

//-----------------------------------------------------------------------------
// Name: Draw()
// Desc: 通常描画 ※行列のX軸方向に頂点が作られ描画される
//-----------------------------------------------------------------------------
void KdTrailPolygon::Draw(float width)
{
	// ポイントが2以下であれば描画負荷(ポリゴンが作れない)
	if (m_pointList.size() < 2) { return; }

	// 軌跡画像の分割数
	float sliceCount = (float)(m_pointList.size() - 1);

	// 頂点情報
	std::vector<Vertex> vertex;

	// ポイント数分サイズ確保
	vertex.resize(m_pointList.size() * 2);

	// ---------------------------------
	// 頂点データ作成
	// ---------------------------------
	for (UINT i = 0; i < m_pointList.size(); i++)
	{
		// 登録行列の参照(ショートカット)
		Math::Matrix& mat = m_pointList[i];

		// 2つの頂点の参照(ショートカット)
		Vertex& v1 = vertex[i * 2];
		Vertex& v2 = vertex[i * 2 + 1];

		// X方向
		Math::Vector3 axisX = mat.Right();
		axisX.Normalize();

		// 座標
		v1.Pos = mat.Translation() + axisX * width * 0.5f;
		v2.Pos = mat.Translation() - axisX * width * 0.5f;

		// UV
		const float uvY = i / sliceCount;
		v1.UV = { 0, uvY };
		v2.UV = { 1, uvY };
	}

	SHADER.m_effectShader.SetWorldMatrix(KdMatrix());
	SHADER.m_effectShader.WriteToCB();

	// テクスチャセット
	auto address = m_texture ? m_texture->GetSRViewAddress() : D3D.GetWhiteTex()->GetSRViewAddress();
	D3D.GetDevContext()->PSSetShaderResources(0, 1, address);

	// 指定した頂点情報を描画する関数
	D3D.DrawVertices(D3D_PRIMITIVE_TOPOLOGY_TRIANGLESTRIP, vertex.size(), &vertex[0], sizeof(Vertex));
}

//-----------------------------------------------------------------------------
// Name: DrawBillboard()
// Desc: ビルボード描画(外積を使用してカメラに向ける)
//-----------------------------------------------------------------------------
void KdTrailPolygon::DrawBillboard(float width)
{
	// ポイントが2以下であれば描画負荷(ポリゴンが作れない)
	if (m_pointList.size() < 2) { return; }

	// カメラの情報
	Math::Matrix mCam = SHADER.m_cb7_Camera.GetWork().mV.Invert();

	// 軌跡画像の分割数
	float sliceCount = (float)(m_pointList.size() - 1);

	// 頂点情報
	std::vector<Vertex> vertex;

	// ポイント数分サイズ確保
	vertex.resize(m_pointList.size() * 2);

	// ---------------------------------
	// 頂点データ作成
	// ---------------------------------
	Math::Vector3 prevPos;
	for (UINT i = 0; i < m_pointList.size(); i++)
	{
		// 登録行列の参照(ショートカット)
		Math::Matrix& mat = m_pointList[i];

		// 2つの頂点の参照(ショートカット)
		Vertex& v1 = vertex[i * 2];
		Vertex& v2 = vertex[i * 2 + 1];

		// ラインの向き
		Math::Vector3 vDir;
		vDir = (i == 0)
			// 初回のみ次のポイントを使用
			? m_pointList[i].Translation() - mat.Translation()
			// 2回目以降は前回の座標から向きを決定
			: mat.Translation() - prevPos;

		// カメラからポイントの向き
		Math::Vector3 v = mat.Translation() - mCam.Translation();
		// 外積使って垂直にできる
		Math::Vector3 axisX = DirectX::XMVector3Cross(vDir, v);
		axisX.Normalize();

		// 座標
		v1.Pos = mat.Translation() + axisX * width * 0.5f;
		v2.Pos = mat.Translation() - axisX * width * 0.5f;

		// UV
		const float uvY = i / sliceCount;
		v1.UV = { 0, uvY };
		v2.UV = { 1, uvY };
	}

	SHADER.m_effectShader.SetWorldMatrix(KdMatrix());
	SHADER.m_effectShader.WriteToCB();

	// テクスチャセット
	auto address = m_texture ? m_texture->GetSRViewAddress() : D3D.GetWhiteTex()->GetSRViewAddress();
	D3D.GetDevContext()->PSSetShaderResources(0, 1, address);

	// 指定した頂点情報を描画する関数
	D3D.DrawVertices(D3D_PRIMITIVE_TOPOLOGY_TRIANGLESTRIP, vertex.size(), &vertex[0], sizeof(Vertex));
}

//-----------------------------------------------------------------------------
// Name: DrawStrip()
// Desc: 頂点情報をそのまま繋げてポリゴンを描画
//-----------------------------------------------------------------------------
void KdTrailPolygon::DrawStrip()
{
	UINT pointListSize = m_pointList.size();
	if (pointListSize < 4) { return; }

	// 頂点情報
	std::vector<Vertex> vertex = {};
	vertex.resize(pointListSize);

	// 軌跡画像の分割数
	float sliceNum = pointListSize * 0.5f;

	// 頂点データ作成
	for (UINT i = 0; i < pointListSize; i++)
	{
		Vertex& rVertex = vertex[i];

		// 頂点座標
		rVertex.Pos = m_pointList[i].Translation();

		// UV
		// X座標は頂点として画像の両端 0.0と1.0を往復する
		rVertex.UV.x = (float)(i % 2);
		rVertex.UV.y = (i * 0.5f) / sliceNum;
	}

	// テクスチャセット
	auto address = m_texture != nullptr ? m_texture->GetSRViewAddress(): D3D.GetWhiteTex()->GetSRViewAddress();
	D3D.GetDevContext()->PSSetShaderResources(0, 1, address);

	// 指定した頂点配列を描画する関数
	D3D.DrawVertices(D3D_PRIMITIVE_TOPOLOGY_TRIANGLESTRIP, vertex.size(), &vertex[0], sizeof(Vertex));
}