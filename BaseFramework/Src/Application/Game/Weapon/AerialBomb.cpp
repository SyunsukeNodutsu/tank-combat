//-----------------------------------------------------------------------------
// File: AerialBomb.cpp
//
// Edit: 2021/03/07 野筒隼輔
//-----------------------------------------------------------------------------
#include "AerialBomb.h"
#include "Application/Game/AI/Character/AutomaticTank.h"
#include "Application/Game/Player/PlayerTank.h"
#include "Application/Game/Other/AnimationEffect.h"
#include "Application/Manager/ObjectManager.h"
#include "Application/Component/CollisionComponent.h"

//-----------------------------------------------------------------------------
// Name: AerialBomb()
// Desc: コンストラクタ
//-----------------------------------------------------------------------------
AerialBomb::AerialBomb()
	: m_spExpSE(nullptr)
	, m_prevPosition(KdVector3(0.0f, 0.0f, 0.0f))
	, m_axis(KdVector3(0.0f, -1.0f, 0.0f))
	, m_speed(0.6f)
	, m_gravity(0.006f)
{
}

//-----------------------------------------------------------------------------
// Name: Deserialize()
// Desc: 初期化
//-----------------------------------------------------------------------------
void AerialBomb::Deserialize(const json11::Json& json_object)
{
	// 基底クラスの初期化(共通の初期化)
	GameObject::Deserialize(json_object);

	// 効果音読み込み
	m_spExpSE = AUDIO.GetSoundInstance3D("Data/Audio/SE/Explosion01.wav");
}

//-----------------------------------------------------------------------------
// Name: Update()
// Desc: 更新
//-----------------------------------------------------------------------------
void AerialBomb::Update()
{
	// 移動前の座標を覚える
	m_prevPosition = m_worldMatrix.GetTranslation();

	// 移動設定
	m_axis.Normalize();
	m_axis *= m_speed;// ※爆弾の速度は結構ゆっくり

	// 移動(落下)
	m_axis.y -= m_gravity;
	m_worldMatrix.Move(m_axis);

	// 判定
	UpdateCollision();
}

//-----------------------------------------------------------------------------
// Name: Emit()
// Desc: 発生
//-----------------------------------------------------------------------------
void AerialBomb::Emit(const KdVector3 a_pos)
{
	// 座標を設定
	m_worldMatrix.SetTranslation(a_pos);
}

//-----------------------------------------------------------------------------
// Name: UpdateCollision()
// Desc: 衝突判定
//-----------------------------------------------------------------------------
void AerialBomb::UpdateCollision()
{
	//--------------------------------------------------
	// 衝突判定用データ作成
	//--------------------------------------------------

	// レイ判定用
	RayInfo rayInfo = {};
	rayInfo.m_pos = m_worldMatrix.GetTranslation();
	rayInfo.m_dir = m_worldMatrix.GetTranslation() - m_prevPosition;// 1フレームの移動分のみ判定
	rayInfo.m_maxRange = rayInfo.m_dir.Length();
	rayInfo.m_dir.Normalize();

	// レイ判定の結果を受け取る
	KdRayResult rayResult;

	//--------------------------------------------------
	// 全GameObjectと判定
	//--------------------------------------------------
	for (auto& object : OBJ_MAGER.GetObjectList()) {
		// TAG確認.StageObjecのみと判定
		if (object->GetTag() & OBJECT_TAG::TAG_StageObject) {
			if (!object->GetCollisionComponent()) { continue; }
			if (object->GetCollisionComponent()->HitCheckByRay(rayInfo, rayResult) == false) { continue; }
			// 着弾
			Landing();
		}
	}
}

//-----------------------------------------------------------------------------
// Name: Landing()
// Desc: 着弾
//-----------------------------------------------------------------------------
void AerialBomb::Landing()
{
	// 爆発
	Explosion();

	//--------------------------------------------------
	// 特定のGameObjectに通知を送る
	//--------------------------------------------------
	for (auto& object : OBJ_MAGER.GetObjectList()) {
		if (object.get() == this) { continue; }

		// TAG確認 ... 戦車
		if (object->GetTag() & OBJECT_TAG::TAG_Tank) {
			// Tankクラスにダウンキャストして通知を送る
			std::shared_ptr<Tank> tank = std::dynamic_pointer_cast<Tank>(object);
			if (tank) { tank->NoticeAerialBombLanding(m_worldMatrix.GetTranslation()); }
		}
	}
}

//-----------------------------------------------------------------------------
// Name: Explosion()
// Desc: 爆発
//-----------------------------------------------------------------------------
void AerialBomb::Explosion()
{
	// Effect
	KdVector3 centerPos = m_worldMatrix.GetTranslation();
	EFFEKSEER.Play(u"Data/Effekseer/Explosion.efk", KdVector3(centerPos.x + 10, 0.0f, centerPos.z));
	EFFEKSEER.Play(u"Data/Effekseer/Explosion.efk", KdVector3(centerPos.x - 10, 0.0f, centerPos.z));
	EFFEKSEER.Play(u"Data/Effekseer/Explosion.efk", KdVector3(centerPos.x, 0.0f, centerPos.z + 10));
	EFFEKSEER.Play(u"Data/Effekseer/Explosion.efk", KdVector3(centerPos.x, 0.0f, centerPos.z - 10));

	// 効果音
	if (m_spExpSE) {
		m_spExpSE->SetVolume(6.0f);
		m_spExpSE->Play();
		m_spExpSE->SetPosition(centerPos);
	}

	// 消滅
	m_isEnable = false;
}