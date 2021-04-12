//-----------------------------------------------------------------------------
// File: KdSquarePolygon.cpp
//
// Edit: 2021/03/07 野筒隼輔
//-----------------------------------------------------------------------------
#include "KdSquarePolygon.h"

//-----------------------------------------------------------------------------
// Name: KdSquarePolygon()
// Desc: コンストラクタ
//-----------------------------------------------------------------------------
KdSquarePolygon::KdSquarePolygon()
	: m_texture(nullptr)
	, m_animSplitX(1)
	, m_animSplitY(1)
	, m_animPos(0)
	, m_vertex()
{
}

//-----------------------------------------------------------------------------
// Name: Initialize()
// Desc: 初期化
//-----------------------------------------------------------------------------
void KdSquarePolygon::Initialize(float width, float height, const Math::Vector4& color)
{
	// 頂点座標の設定(幅高さ分中央基準で離す)
	m_vertex[0].pos = { -width * 0.5f, -height * 0.5f , 0.0f };// 左下
	m_vertex[1].pos = { -width * 0.5f,  height * 0.5f , 0.0f };// 左上
	m_vertex[2].pos = {  width * 0.5f, -height * 0.5f , 0.0f };// 右下
	m_vertex[3].pos = {  width * 0.5f,  height * 0.5f , 0.0f };// 右上

	m_vertex[0].color = color;
	m_vertex[1].color = color;
	m_vertex[2].color = color;
	m_vertex[3].color = color;

	// UV座標の設定
	m_vertex[0].UV = { 0.0f, 1.0f };// 左下
	m_vertex[1].UV = { 0.0f, 0.0f };// 左上
	m_vertex[2].UV = { 1.0f, 1.0f };// 右下
	m_vertex[3].UV = { 1.0f, 0.0f };// 右上
}

//-----------------------------------------------------------------------------
// Name: Draw()
// Desc: 描画
//-----------------------------------------------------------------------------
void KdSquarePolygon::Draw(int set_textureNo)
{
	// テクスチャ設定
	auto pp_srv = (m_texture) ? m_texture->GetSRViewAddress() : D3D.GetWhiteTex()->GetSRViewAddress();
	D3D.GetDevContext()->PSSetShaderResources(set_textureNo, 1, pp_srv);

	// 指定した頂点配列を描画
	D3D.DrawVertices(D3D_PRIMITIVE_TOPOLOGY_TRIANGLESTRIP, 4, &m_vertex, sizeof(Vertex));
}

//-----------------------------------------------------------------------------
// Name: Animation()
// Desc: アニメーションを進行
//-----------------------------------------------------------------------------
void KdSquarePolygon::Animation(float speed, bool loop)
{
	// スピード分位置を進める
	m_animPos += speed;

	// 終了判定
	if (m_animPos >= (m_animSplitX * m_animSplitY) - 1) {
		m_animPos = (loop) ? 0 : static_cast<float>(m_animSplitX * m_animSplitY) - 1;
	}

	// UV座標の更新
	SetAnimationPos(m_animPos);
}

//-----------------------------------------------------------------------------
// Name: IsAnimationEnd()
// Desc: アニメーションが終了しているか確認
//-----------------------------------------------------------------------------
bool KdSquarePolygon::IsAnimationEnd()
{
	if (m_animPos >= (m_animSplitX * m_animSplitY) - 1) { return true; }
	return false;
}

//-----------------------------------------------------------------------------
// Name: SetAnimationPos()
// Desc: アニメーションの進行具合を設定
//-----------------------------------------------------------------------------
void KdSquarePolygon::SetAnimationPos(float index)
{
	// アニメーションの横縦それぞれのコマ数を計算する
	int x = (int)index % m_animSplitX;
	int y = (int)index / m_animSplitY;

	// 1コマの幅と高さを計算する
	float w = 1.0f / m_animSplitX;
	float h = 1.0f / m_animSplitY;

	// UV座標の計算
	m_vertex[0].UV = { x * w, (y + 1) * h };		// 左下
	m_vertex[1].UV = { x * w, y * h };				// 左上
	m_vertex[2].UV = { (x + 1) * w, (y + 1) * h };	// 右下
	m_vertex[3].UV = { (x + 1) * w, y * h };		// 右上

	// アニメーションの位置を行進
	m_animPos = index;
}