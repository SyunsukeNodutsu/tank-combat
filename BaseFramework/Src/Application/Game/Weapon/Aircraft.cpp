//-----------------------------------------------------------------------------
// File: Aircraft.cpp
//
// Edit: 2021/02/19 野筒隼輔
//-----------------------------------------------------------------------------
#include "Aircraft.h"

Aircraft::Aircraft()
	: m_spPropeller(nullptr)
	, m_localPropMat()
	, m_trailSmoke()
	, m_trailRotate(1.0f)
{
}

void Aircraft::Deserialize(const json11::Json& json_object)
{
	// 共通の初期化
	GameObject::Deserialize(json_object);

	// 飛行機雲テクスチャ
	m_trailSmoke.SetTexture(KdResFac.GetTexture("Data/Texture/Smokeline.png"));
}

void Aircraft::Update()
{
	//--------------------------------------------------
	// 移動
	//--------------------------------------------------

	// 移動ベクトル
	KdVector3 move = KdVector3(0.0f, 0.0f, 1.0f);
	move.Normalize();
	move *= 0.2f;// 移動速度補正

	// ワールド行列に合成
	KdMatrix moveMat = {};
	moveMat.CreateTranslation(move.x, move.y, move.z);
	m_worldMatrix = moveMat * m_worldMatrix;

	//--------------------------------------------------
	// 回転
	//--------------------------------------------------

	// 回転ベクトル
	KdVector3 rotate = KdVector3(-1.0f, 0.0f, 0.0f);

	// ワールド座標に合成
	KdMatrix rotateMat = {};
	rotateMat.CreateRotationX(rotate.x * KdToRadians);
	rotateMat.RotateZ(rotate.z * KdToRadians);
	m_worldMatrix = rotateMat * m_worldMatrix;

	//--------------------------------------------------
	// 飛行機雲
	//--------------------------------------------------

	// 軌跡の座標を先頭に追加
	m_trailSmoke.AddPoint(m_worldMatrix);

	const int LIST_MAX = 160;
	if (m_trailSmoke.GetNumPoints() > LIST_MAX) {
		// 一番後ろを消す
		m_trailSmoke.DeletePoint_Back();
	}
}

void Aircraft::DrawEffect()
{
	// 飛行機雲をビルボード描画
	m_trailSmoke.DrawBillboard(0.5f);
}