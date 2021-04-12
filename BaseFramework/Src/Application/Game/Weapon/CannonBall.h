//-----------------------------------------------------------------------------
// File: CannonBall.h
//
// Edit: 2021/03/19 野筒隼輔
//-----------------------------------------------------------------------------
#pragma once
#include "Application/Game/GameObject.h"

//--------------------------------------------------
// 前方宣言
//--------------------------------------------------
struct RayInfo;

//-----------------------------------------------------------------------------
// Name: class CannonBall
// Desc: 戦車の砲弾クラス
//-----------------------------------------------------------------------------
class CannonBall : public GameObject
{
public:

	// コンストラクタ
	CannonBall(GameObject& owner);
	// 更新
	void Update() override;
	// 発射
	void Fire(const KdVector3 pos, const KdVector3 axis);

private:
	GameObject& m_owner;		// オーナー
	KdVector3	m_prevPosition;	// 1フレーム前の座標
	KdVector3	m_axis;			// 進行方向
	float		m_speed;		// 移動速度
	int			m_lifeSpan;		// 生存期間

	// 効果音
	std::shared_ptr<SoundInstance3D> m_spExplosionSE;

private:
	void UpdateCollision();
	void Explosion();
	const bool CheckStageObject(std::shared_ptr<GameObject> object, RayInfo ray_info, KdRayResult& ray_result);
	const bool CheckGrassObject(std::shared_ptr<GameObject> object, RayInfo ray_info, KdRayResult& ray_result);
	const bool CheckTankObject(std::shared_ptr<GameObject> object, RayInfo ray_info, KdRayResult& ray_result);
};