//-----------------------------------------------------------------------------
// File: CheckObstacleBall.h
//
// Edit: 2021/04/10 野筒隼輔
//-----------------------------------------------------------------------------
#include "CheckObstacleBall.h"
#include "Application/Game/Weapon/Tank.h"
#include "Application/Component/CameraComponent.h"
#include "Application/Component/CollisionComponent.h"
#include "Application/Component/ModelComponent.h"
#include "Application/Manager/ObjectManager.h"

//-----------------------------------------------------------------------------
// Name: CheckObstacleBall()
// Desc: コンストラクタ
//-----------------------------------------------------------------------------
CheckObstacleBall::CheckObstacleBall(Tank& owner)
	: m_owner(owner)
	, m_prevPosition(KdVector3(0.0f, 0.0f, 0.0f))
	, m_axis(KdVector3(0.0f, 0.0f, 0.0f))
	, m_speed(7.0f)
	, m_lifeSpan(300)
{
	m_worldMatrix.SetTranslation(m_owner.GetMatrix().GetTranslation() + KdVector3(0.0f, 0.2f, 0.0f));
}

//-----------------------------------------------------------------------------
// Name: Update()
// Desc: 更新
//-----------------------------------------------------------------------------
void CheckObstacleBall::Update()
{
	if (!m_isAlive) { return; }

	// 移動前の座標記憶
	m_prevPosition = m_worldMatrix.GetTranslation();

	// 移動設定
	m_axis.Normalize();
	m_axis *= m_speed;

	// 移動
	m_worldMatrix.Move(m_axis);

	// 衝突判定更新
	UpdateCollision();

	// 有効期間
	if (--m_lifeSpan <= 0) { m_isAlive = false; }
}

//-----------------------------------------------------------------------------
// Desc: 
//-----------------------------------------------------------------------------
HitResult CheckObstacleBall::Result()
{
	return HitResult();
}

//-----------------------------------------------------------------------------
// Name: UpdateCollision()
// Desc: 衝突判定更新 戦車に衝突したら発見
//-----------------------------------------------------------------------------
void CheckObstacleBall::UpdateCollision()
{
	RayInfo ray_info = {};
	ray_info.m_pos = m_worldMatrix.GetTranslation();
	ray_info.m_dir = m_worldMatrix.GetTranslation() - m_prevPosition;
	ray_info.m_maxRange = ray_info.m_dir.Length();
	ray_info.m_dir.Normalize();

	KdRayResult ray_result = {};

	// 全Object
	for (auto& object : OBJ_MAGER.GetObjectList()) {
		if (object.get() == this) { continue; }
		// 障害物
		if (object->GetTag() & (OBJECT_TAG::TAG_StageObject | OBJECT_TAG::TAG_HouseObject)) {
			if (object->GetCollisionComponent()->HitCheckByRay(ray_info, ray_result)) {
				m_isAlive = false;
				break;
			}
		}
		// 目標(戦車)
		if (object->GetTag() & OBJECT_TAG::TAG_Tank) {
			// すでに発見されていたら飛ばす ※Tankにダウンキャストして確認
			std::shared_ptr<Tank> tank = std::dynamic_pointer_cast<Tank>(object);
			if (tank == nullptr) { assert(0 && "error cast"); }
			if (tank->IsDiscovery()) { continue; }
			if (tank->GetCollisionComponent()->HitCheckByRay(ray_info, ray_result)) {
				m_isAlive = false;
				CheckTank(tank);
				break;
			}
		}
	}
}

//-----------------------------------------------------------------------------
// Name: CheckTank()
// Desc: 戦車との判定
//-----------------------------------------------------------------------------
void CheckObstacleBall::CheckTank(std::shared_ptr<Tank> object)
{
	// 対象のメッシュ所得
	KdMesh* pMesh = object->GetCollModelComponent()->GetMesh(0).get();
	if (pMesh == nullptr) { return; }

	// メッシュのAABBから物体の行列も考慮してOBBへ変換
	DirectX::BoundingOrientedBox obb = {};
	DirectX::BoundingOrientedBox::CreateFromBoundingBox(obb, pMesh->GetBoundingBox());
	obb.Transform(obb, object->GetMatrix());

	// 視錐台の範囲内か確認
	if (!m_owner.GetCameraComponent()->GetFrustum().Intersects(obb)) { return; }

	// 発見を対象に通知
	object->Discovery();
}