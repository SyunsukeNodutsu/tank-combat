//-----------------------------------------------------------------------------
// File: AnimationEffect.h
//
// Edit: 2021/03/18 野筒隼輔
//-----------------------------------------------------------------------------
#pragma once
#include "Application/Game/GameObject.h"

//-----------------------------------------------------------------------------
// Name: class AnimationEffect
// Desc: 画像を矩形ごとに区切りアニメーションを行うクラス
//-----------------------------------------------------------------------------
class AnimationEffect : public GameObject
{
public:

	// コンストラクタ
	AnimationEffect();
	// アニメーション情報設定
	void SetAnimationInfo(const std::shared_ptr<KdTexture>& texture, float size, float splitX, float splitY, float zAngle, float speed, bool loop = false);
	// 更新
	void Update() override;
	// Effect描画
	void DrawEffect() override;

private:
	KdSquarePolygon m_poly;		// ポリゴン
	float			m_angleZ;	// Z軸の回転
	float			m_speed;	// 速度
	bool			m_isLoop;	// ループするか
};