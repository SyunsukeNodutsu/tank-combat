//-----------------------------------------------------------------------------
// File: KdTrailPolygon.h
//
// Edit: 2021/03/07 野筒隼輔
//-----------------------------------------------------------------------------
#pragma once

//-----------------------------------------------------------------------------
// Name: class KdTrailPolygon
// Desc: エフェクト用のポリゴンクラス
//-----------------------------------------------------------------------------
class KdTrailPolygon
{
public:

	// 通常描画 ※width...ポリゴンの幅
	void Draw(float width);

	// ビルボード描画
	void DrawBillboard(float width);

	// 頂点情報をそのまま繋げてポリゴンを描画
	void DrawStrip();

	//--------------------------------------------------
	// 所得
	//--------------------------------------------------
	// 先頭のポイントを所得
	inline Math::Matrix* GetTopPoint()
	{
		if (m_pointList.size() == 0) { return nullptr; }
		return &m_pointList.front();
	}
	// リストの数を所得
	inline int GetNumPoints() const { return (int)m_pointList.size(); }

	//--------------------------------------------------
	// 設定
	//--------------------------------------------------
	// 画像をセット
	inline void SetTexture(const std::shared_ptr<KdTexture>& tex) { m_texture = tex; }
	// ポイントを追加
	inline void AddPoint(const Math::Matrix& mat) { m_pointList.push_front(mat); }
	// 最後尾のポイントを削除
	inline void DeletePoint_Back() { m_pointList.pop_back(); }

private:
	std::shared_ptr<KdTexture>	m_texture;		// テクスチャ
	std::deque<Math::Matrix>	m_pointList;	// 軌跡の位置などを記憶する行列配列

	// 1頂点の形式
	struct Vertex
	{
		Math::Vector3 Pos;
		Math::Vector2 UV;
		Math::Vector4 Color = Math::Vector4(1.0f, 1.0f, 1.0f, 1.0f);
	};
};