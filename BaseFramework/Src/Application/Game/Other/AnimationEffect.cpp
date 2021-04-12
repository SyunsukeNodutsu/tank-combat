//-----------------------------------------------------------------------------
// File: AnimationEffect.cpp
//
// Edit: 2021/03/18 野筒隼輔
//-----------------------------------------------------------------------------
#include "AnimationEffect.h"

//-----------------------------------------------------------------------------
// Name: AnimationEffect()
// Desc: コンストラクタ
//-----------------------------------------------------------------------------
AnimationEffect::AnimationEffect()
	: m_poly({})
	, m_angleZ(0)
	, m_speed(0.5f)
	, m_isLoop(false)
{
	m_name = "Effect";
}

//-----------------------------------------------------------------------------
// Name: SetAnimationInfo()
// Desc: 初期設定
//-----------------------------------------------------------------------------
void AnimationEffect::SetAnimationInfo(const std::shared_ptr<KdTexture>& texture, float size, float splitX, float splitY, float angleZ, float speed, bool loop)
{
	// ポリゴン
	m_poly.Initialize(size, size, Math::Vector4(1.0f, 1.0f, 1.0f, 1.0f));
	m_poly.SetAnimationInfo(static_cast<int>(splitX), static_cast<int>(splitY));
	m_poly.SetAnimationPos(0);
	m_poly.SetTexture(texture);
	
	m_angleZ = angleZ;
	m_speed  = speed;
	m_isLoop = loop;
}

//-----------------------------------------------------------------------------
// Name: Update()
// Desc: 更新
//-----------------------------------------------------------------------------
void AnimationEffect::Update()
{
	// アニメーション進行
	m_poly.Animation(m_speed, m_isLoop);
	// 終了
	if (m_poly.IsAnimationEnd()) { Exclusion(); }
}

//-----------------------------------------------------------------------------
// Name: DrawEffect()
// Desc: Effect描画
//-----------------------------------------------------------------------------
void AnimationEffect::DrawEffect()
{
	// 各軸方向の拡大率を所得
	float scaleX = m_worldMatrix.GetAxisX().Length();
	float scaleY = m_worldMatrix.GetAxisY().Length();
	float scaleZ = m_worldMatrix.GetAxisZ().Length();

	// ビルボード処理
	KdMatrix drawMat = {};
	drawMat.CreateScalling(scaleX, scaleY, scaleZ);	// 拡大率を設定
	drawMat.RotateZ(m_angleZ * KdToRadians);		// Z軸の回転角度を加える

	// 座標は自身の
	drawMat.SetTranslation(m_worldMatrix.GetTranslation());

	// 描画行列にビルボード回転 ※移動成分を含んでいてもOK
	drawMat.SetBillboard(SHADER.m_cb7_Camera.GetWork().mV);

	// 描画
	SHADER.m_effectShader.SetWorldMatrix(drawMat);
	SHADER.m_effectShader.WriteToCB();
	m_poly.Draw(0);
}