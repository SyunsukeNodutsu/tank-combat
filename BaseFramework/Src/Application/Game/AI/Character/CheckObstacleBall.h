//-----------------------------------------------------------------------------
// File: CheckObstacleBall.h
//
// Edit: 2021/04/10 野筒隼輔
//-----------------------------------------------------------------------------
#pragma once
#include "Application/Game/GameObject.h"

//--------------------------------------------------
// 前方宣言
//--------------------------------------------------
class Tank;

//-----------------------------------------------------------------------------
// Name: class CheckObstacleBall
// Desc: 敵に見えない球を発射して 衝突したら発見とみなす
//-----------------------------------------------------------------------------
class CheckObstacleBall : public GameObject
{
public:

	// コンストラクタ
	CheckObstacleBall(Tank& owner);
	// 更新
	void Update() override;
	// 軸設定
	void SetAxis(const KdVector3 axis) { m_axis = axis; }

private:
	Tank&		m_owner;		// オーナー
	KdVector3	m_prevPosition;	// 1フレーム前の座標
	KdVector3	m_axis;			// 進行方向
	float		m_speed;		// 移動速度
	int			m_lifeSpan;		// 生存期間

private:
	void UpdateCollision();
	void CheckTank(std::shared_ptr<Tank> object);
};