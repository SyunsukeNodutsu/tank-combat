//-----------------------------------------------------------------------------
// File: CannonBall.cpp
//
// Edit: 2021/03/19 野筒隼輔
//-----------------------------------------------------------------------------
#include "CannonBall.h"
#include "Application/Game/Other/AnimationEffect.h"
#include "Application/Game/Other/Radio.h"
#include "Application/Game/Weapon/Tank.h"

#include "Application/Component/CollisionComponent.h"
#include "Application/Manager/CameraManager.h"
#include "Application/Manager/ObjectManager.h"

//-----------------------------------------------------------------------------
// Name: CannonBall()
// Desc: コンストラクタ
//-----------------------------------------------------------------------------
CannonBall::CannonBall(GameObject& owner)
	: m_owner(owner)
	, m_prevPosition(KdVector3(0.0f, 0.0f, 0.0f))
	, m_axis(KdVector3(0.0f, 0.0f, 0.0f))
	, m_speed(7.0f)
	, m_lifeSpan(300)
{
}

//-----------------------------------------------------------------------------
// Name: Update()
// Desc: 更新
//-----------------------------------------------------------------------------
void CannonBall::Update()
{
	if (!m_isAlive) { return; }

	// 移動前の座標記憶
	m_prevPosition = m_worldMatrix.GetTranslation();

	// 移動設定
	m_axis.Normalize();
	m_axis *= m_speed;

	// 移動
	constexpr float gravity = 0.006f;
	m_axis.y -= gravity;
	m_worldMatrix.Move(m_axis);

	// 衝突判定更新
	UpdateCollision();

	// 有効期間
	if (--m_lifeSpan <= 0) { Explosion(); }
}

//-----------------------------------------------------------------------------
// Name: Fire()
// Desc: 射撃
//-----------------------------------------------------------------------------
void CannonBall::Fire(const KdVector3 pos, const KdVector3 axis)
{
	// 初期座標
	m_worldMatrix.SetTranslation(pos);
	// 進行方向
	m_axis = axis;

	// 効果音読み込み
	m_spExplosionSE = AUDIO.GetSoundInstance3D("Data/Audio/SE/Explosion.wav", m_worldMatrix.GetTranslation());
}

//-----------------------------------------------------------------------------
// Name: UpdateCollision()
// Desc: 衝突判定更新
//-----------------------------------------------------------------------------
void CannonBall::UpdateCollision()
{
	// レイ判定用
	RayInfo ray_info = {};
	ray_info.m_pos = m_worldMatrix.GetTranslation();
	ray_info.m_dir = m_worldMatrix.GetTranslation() - m_prevPosition;
	ray_info.m_maxRange = ray_info.m_dir.Length();
	ray_info.m_dir.Normalize();

	// レイ判定の結果を受け取る
	KdRayResult ray_result = {};

	// 全GameObject
	for (auto& object : OBJ_MAGER.GetObjectList())
	{
		if (object.get() == this) { continue; }

		// TAG_StageObject
		if (object->GetTag() & OBJECT_TAG::TAG_StageObject) {
			if (CheckStageObject(object, ray_info, ray_result)) { return; }
		}

		// TAG_Tank
		if (object->GetTag() & OBJECT_TAG::TAG_Tank) {
			if (CheckTankObject(object, ray_info, ray_result)) { return; }
		}

		// TAG_HouseObject
		if (object->GetTag() & OBJECT_TAG::TAG_HouseObject) {
			if (CheckStageObject(object, ray_info, ray_result)) { return; }
		}
	}
}

//-----------------------------------------------------------------------------
// Name: Explosion()
// Desc: 爆発
//-----------------------------------------------------------------------------
void CannonBall::Explosion()
{
	m_isAlive  = m_isEnable = false;

	// アニメーションエフェクト 生成.設定
	std::shared_ptr<AnimationEffect> effect = std::make_shared<AnimationEffect>();
	if (effect) {
		effect->SetAnimationInfo(KdResFac.GetTexture("Data/Texture/Effect/Explosion.png"), 20.0f, 5, 5, static_cast<float>(rand() % 360), 0.5f);
		effect->SetMatrix(m_worldMatrix);
		OBJ_MAGER.AddObject(effect);
	}

	// 効果音
	if (m_spExplosionSE) {
		m_spExplosionSE->Play();
		m_spExplosionSE->SetPosition(m_worldMatrix.GetTranslation());
	}
}

//-----------------------------------------------------------------------------
// Name: CheckStageObject()
// Desc: 地面との判定
//-----------------------------------------------------------------------------
const bool CannonBall::CheckStageObject(std::shared_ptr<GameObject> object, RayInfo ray_info, KdRayResult& ray_result)
{
	if (object->GetCollisionComponent() == nullptr) { return false; }
	const bool bHit = object->GetCollisionComponent()->HitCheckByRay(ray_info, ray_result);
	if (bHit) { Explosion(); }

	return bHit;
}

//-----------------------------------------------------------------------------
// Name: CheckGrassObject()
// Desc: 草の壁との判定
//-----------------------------------------------------------------------------
const bool CannonBall::CheckGrassObject(std::shared_ptr<GameObject> object, RayInfo ray_info, KdRayResult& ray_result)
{
	if (object->GetCollisionComponent() == nullptr) { return false; }
	const bool bHit = object->GetCollisionComponent()->HitCheckByRay(ray_info, ray_result);
	if (bHit) { Explosion(); }

	return bHit;
}

//-----------------------------------------------------------------------------
// Name: CheckTankObject()
// Desc: 戦車との判定
//-----------------------------------------------------------------------------
const bool CannonBall::CheckTankObject(std::shared_ptr<GameObject> object, RayInfo ray_info, KdRayResult& ray_result)
{
	if (object->GetCollisionComponent() == nullptr) { return false; }

	const bool bHit = object->GetCollisionComponent()->HitCheckByRay(ray_info, ray_result);

	//--------------------------------------------------
	// TAG確認 Player
	//--------------------------------------------------
	if (m_owner.GetTag() & OBJECT_TAG::TAG_Player) {
		// 敵を攻撃
		if (object->GetTag() & OBJECT_TAG::TAG_Enemy) {
			if (bHit) {
				// Tankクラスにダウンキャストして通知
				std::shared_ptr<Tank> tank = std::dynamic_pointer_cast<Tank>(m_owner.shared_from_this());
				// 命中を通知
				if (tank) { tank->AttackHit(); }
				if (object->IsAlive()) {
					// 撃破通知
					if (tank) { tank->EnemyDefeats(); }
					CAMERA_MAGER.StartAnimation(m_owner.shared_from_this(), object->GetName());
					// カメラアニメーションを行わないか確認
					if (!CAMERA_MAGER.IsAnimationEnable()) {
						// 無線.撃破したぞ！
						RADIO.Emit(WIRELESS_TYPE::eDefeat, OWNER_TYPE::ePlayer, true);
					}
				}
				object->Destroy();
				m_isAlive = m_isEnable = false;
			}
		}
		// 味方を攻撃 ※フレンドリーファイア
		if (object->GetTag() & OBJECT_TAG::TAG_Allies) {
			if (bHit) {
				object->Destroy();
				m_isAlive = m_isEnable = false;
			}
		}
	}

	//--------------------------------------------------
	// TAG確認 Allies
	//--------------------------------------------------
	if (m_owner.GetTag() & OBJECT_TAG::TAG_Allies	) {
		// 敵を攻撃
		if (object->GetTag() & OBJECT_TAG::TAG_Enemy) {
			if (bHit) {
				object->Destroy();
				m_isAlive = m_isEnable = false;
			}
		}
		// プレイヤーを攻撃 ※フレンドリーファイア
		if (object->GetTag() & OBJECT_TAG::TAG_Player) {
			if (bHit) {
				object->Destroy();
				m_isAlive = m_isEnable = false;
			}
		}
	}

	//--------------------------------------------------
	// TAG確認 Enemy
	//--------------------------------------------------
	if (m_owner.GetTag() & (OBJECT_TAG::TAG_Enemy | OBJECT_TAG::TAG_Allies)) {
		// 敵を攻撃
		if (object->GetTag() & OBJECT_TAG::TAG_Player) {
			if (bHit) {
				object->Destroy();
				m_isAlive = m_isEnable = false;
			}
		}
		// 味方を攻撃 ※フレンドリーファイア
		if (object->GetTag() & OBJECT_TAG::TAG_Enemy) {
			/*if (bHit) {
				object->Destroy();
				m_isAlive = m_isEnable = false;
			}*/
		}
	}

	return bHit;
}