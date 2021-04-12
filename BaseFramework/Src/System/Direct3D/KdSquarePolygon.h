//-----------------------------------------------------------------------------
// File: KdSquarePolygon.h
//
// Edit: 2021/03/07 野筒隼輔
//-----------------------------------------------------------------------------
#pragma once

//-----------------------------------------------------------------------------
// Name: class KdSquarePolygon
// Desc: 四角形ポリゴンクラス
//-----------------------------------------------------------------------------
class KdSquarePolygon
{
public:

	// コンストラクタ
	KdSquarePolygon();

	// 初期化.ポリゴン情報の設定
	void Initialize(float width, float height, const Math::Vector4& color);

	// ポリゴンの描画
	void Draw(int set_textureNo);

	// アニメーションを進行させる
	void Animation(float speed, bool loop);

	//--------------------------------------------------
	// 所得
	//--------------------------------------------------
	// アニメーションが終わったかどうか
	bool IsAnimationEnd();

	//--------------------------------------------------
	// 設定
	//--------------------------------------------------
	// 使用するテクスチャの設定
	inline void SetTexture(const std::shared_ptr<KdTexture>& texture) { m_texture = texture; }
	// 分割設定,渡されたコマ数を元にUV座標の計算
	void SetAnimationPos(float index);
	// アニメーションの分割数を決定
	inline void SetAnimationInfo(int splitX, int splitY) { m_animSplitX = splitX; m_animSplitY = splitY; }

private:
	std::shared_ptr<KdTexture>	m_texture;		// 貼り付けるテクスチャ情報
	int							m_animSplitX;	// 横の分割数
	int							m_animSplitY;	// 縦の分割数
	float						m_animPos;		// 現在のコマ数

	// 1頂点の情報
	struct Vertex
	{
		Math::Vector3 pos;	// 3D空間上の座標
		Math::Vector2 UV;	// テクスチャ座標(0～1)
		Math::Vector4 color;// 頂点の色(2点間は補完される)
	};
	Vertex m_vertex[4];	// 四角形の情報
};