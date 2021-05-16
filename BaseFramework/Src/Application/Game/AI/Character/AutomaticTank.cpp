//-----------------------------------------------------------------------------
// File: AutomaticTank.h
//
// Edit: 2021/05/14 野筒隼輔
//-----------------------------------------------------------------------------
#include "AutomaticTank.h"
#include "Application/Game/AI/Character/CheckObstacleBall.h"
#include "Application/Game/Weapon/TankParts.h"
#include "Application/Component/CameraComponent.h"
#include "Application/Manager/ClearManager.h"
#include "Application/Manager/DrawManager.h"
#include "Application/Manager/DebugManager.h"
#include "Application/Manager/ObjectManager.h"

//-----------------------------------------------------------------------------
// Name: AutomaticTank()
// Desc: コンストラクタ
//-----------------------------------------------------------------------------
AutomaticTank::AutomaticTank()
	: m_spState(nullptr)
	, m_wayPoint()
	, m_wayPointCount(0)
	, m_nextWayPoint(KdVector3(0.0f, 0.0f, 0.0f))
	, m_searchRange(150.0f)
	, m_onceRotFlg(false)
	, m_enemyTarget(nullptr)
	, m_checkObstacleBall(nullptr)
{
}

//-----------------------------------------------------------------------------
// Name: Deserialize()
// Desc: 初期化
//-----------------------------------------------------------------------------
void AutomaticTank::Deserialize(const json11::Json& json_object)
{
	Tank::Deserialize(json_object);
	bool hasWayPoint = SettingWayPoint(json_object);

	// カメラ初期Y回転
	if (!json_object["Rotation"].is_null())
	{
		const std::vector<json11::Json>& rotate = json_object["Rotation"].array_items();
		if (rotate.size() == 3 && m_spCameraComponent)
		{
			m_spCameraComponent->OffsetMatrix().CreateTranslation(0.0f, 3.0f, -8.0f);
			m_spCameraComponent->OffsetMatrix().RotateY(static_cast<float>(rotate[1].number_value()) * KdToRadians);
		}
	}

	if (hasWayPoint)
		// 前進命令
		AdvanceOrder(m_wayPoint[m_wayPointCount]);
	else
		// 経由地点がなければ待機
		ChangeWait();
}

//-----------------------------------------------------------------------------
// Name: UpdateByState()
// Desc: 状態ごとの更新
//-----------------------------------------------------------------------------
void AutomaticTank::UpdateByState()
{
	// 状態ごとの更新
	if (m_spState)
		m_spState->Update(*this);

	// 障害物確認球の更新
	if (m_checkObstacleBall)
		m_checkObstacleBall->Update();

	// 経由地点表示
	if (DEBUG_MAGER.IsPointListShow())
	{
		for (auto& point : m_wayPoint)
		{
			KdVector3 upper_right = point.second + KdVector3::GetLaunchPoint(0, 1.2f);
			KdVector3 upper_left = point.second + KdVector3::GetLaunchPoint(90, 1.2f);
			KdVector3 lower_right = point.second + KdVector3::GetLaunchPoint(180, 1.2f);
			KdVector3 lower_left = point.second + KdVector3::GetLaunchPoint(270, 1.2f);
			KdVector3 top = point.second + KdVector3(0.0f, 4.0f, 0.0f);

			DRAW_MAGER.AddDebugLine(upper_right, upper_left, kRedColor);
			DRAW_MAGER.AddDebugLine(upper_left, lower_right, kRedColor);
			DRAW_MAGER.AddDebugLine(lower_right, lower_left, kRedColor);
			DRAW_MAGER.AddDebugLine(lower_left, upper_right, kRedColor);

			DRAW_MAGER.AddDebugLine(upper_right, top, kRedColor);
			DRAW_MAGER.AddDebugLine(upper_left, top, kRedColor);
			DRAW_MAGER.AddDebugLine(lower_right, top, kRedColor);
			DRAW_MAGER.AddDebugLine(lower_left, top, kRedColor);
		}
	}
}

//-----------------------------------------------------------------------------
// Name: Destroy()
// Desc: 死亡
//-----------------------------------------------------------------------------
void AutomaticTank::Destroy()
{
	// エフェクト
	KdVector3 effectPos = m_position;
	effectPos.y -= 0.4f;
	if (m_deadExpEffect)
		m_deadExpEffect->Play(effectPos);

	// 効果音
	AUDIO.Play3D("Data/Audio/SE/Explosion.wav", m_position);

	// 死体撃ちで消滅
	if (!m_isAlive)
	{
		Exclusion();
		if (m_deadSmkEffect)
			m_deadSmkEffect->Stop();
		return;
	}

	// 状態.死亡へ
	m_spState = std::make_shared<StateDead>();
	m_spState->Initialize(*this);

	// 部品が壊れる
	if (m_spTankParts)
		m_spTankParts->Crash();

	// 撃破通知
	if (m_tag & OBJECT_TAG::TAG_Enemy)
		CLEAR_MAGER.EnemyDefeat();

	GameObject::Destroy();
}

//-----------------------------------------------------------------------------
// Name: AdvanceOrder()
// Desc: 前進命令
//-----------------------------------------------------------------------------
void AutomaticTank::AdvanceOrder(const KdVector3 position)
{
	// 目標座標を設定
	m_nextWayPoint = position;
	ChangeMission();
}

//-----------------------------------------------------------------------------
// Name: CheckNextPoint()
// Desc: 次の経由地点を確認
//-----------------------------------------------------------------------------
void AutomaticTank::CheckNextWayPoint()
{
	// 到着しているか確認
	bool isArrival = CheckArrivalWayPoint();
	if (!isArrival)
		return;

	// 到着していたらカウント ※次の経由地点
	m_wayPointCount++;

	// 経由地点が残っている -> 引き続き前進命令
	if (m_wayPointCount < m_wayPoint.size())
		AdvanceOrder(m_wayPoint[m_wayPointCount]);
	// 経由地点が残っていない(最後まで到着) -> 待機状態に遷移
	else
		ChangeWait();
}

//-----------------------------------------------------------------------------
// Name: UpdateRotateBodyEx()
// Desc: 引数の軸に車体を回転させる ※修正.回転時にUVスクロールするように rotatePow
//-----------------------------------------------------------------------------
const bool AutomaticTank::UpdateRotateBodyEx(const KdVector3 axis)
{
	// 現在のZ軸
	KdVector3 nowDir = m_worldMatrix.GetAxisZ();
	nowDir.Normalize();

	// 差分を算出
	float rotateRadian = atan2f(axis.x, axis.z) - atan2f(nowDir.x, nowDir.z);

	// 180度の角度で数値の切れ目があるので補正
	if (rotateRadian > M_PI)
		rotateRadian -= 2 * static_cast<float>(M_PI);
	else if (rotateRadian < -M_PI)
		rotateRadian += 2 * static_cast<float>(M_PI);

	// 最大値以内に補正
	const float rotateMax = 20.0f * KdToRadians;
	rotateRadian = std::clamp(rotateRadian, -rotateMax, rotateMax);

	// 回転行列作成
	KdMatrix rotateMat = {};
	rotateMat.CreateRotationX(0);
	rotateMat.RotateY(rotateRadian * KdToRadians);

	// ワールド行列に合成
	m_worldMatrix = rotateMat * m_worldMatrix;

	// 誤差で許す範囲
	const float tolerance = 0.4f * KdToRadians;
	return (rotateRadian <= tolerance && rotateRadian >= -tolerance)
		? true		// 回転完了
		: false;	// 回転中
}

//-----------------------------------------------------------------------------
// Mame: SettingWayPoint()
// Desc: 経由地点の設定
//-----------------------------------------------------------------------------
const bool AutomaticTank::SettingWayPoint(const json11::Json& json_object)
{
	if (json_object.is_null())
		return false;

	if (json_object["WayPointList"].is_null())
		return false;

	auto& objectDataList = json_object["WayPointList"].array_items();
	uint8_t index = 0;

	for (auto&& objJsonData : objectDataList)
	{
		if (objJsonData["WayPoint"].is_null())
			continue;

		const std::vector<json11::Json>& position = objJsonData["WayPoint"].array_items();
		if (position.size() == 3)
		{
			m_wayPoint[index] = KdVector3(
				static_cast<float>(position[0].number_value()),
				static_cast<float>(position[1].number_value()),
				static_cast<float>(position[2].number_value())
			);
		}
		index++;
	}

	return true;
}

//-----------------------------------------------------------------------------
// Name: CheckArrivalWayPoint()
// Desc: 経由地点に到着したか確認
//-----------------------------------------------------------------------------
const bool AutomaticTank::CheckArrivalWayPoint()
{
	// 許容する値
	constexpr float tolerance = 4.0f;

	// 距離判定
	return m_nextWayPoint.x - m_position.x <= tolerance && m_nextWayPoint.x - m_position.x >= -tolerance &&
		m_nextWayPoint.z - m_position.z <= tolerance && m_nextWayPoint.z - m_position.z >= -tolerance;
}

//-----------------------------------------------------------------------------
// Name: CheckObstacle(std::shared_ptr<Tank>)
// Desc: 敵との間に障害物がないかを確認
//-----------------------------------------------------------------------------
const bool AutomaticTank::CheckObstacle(std::shared_ptr<Tank> enemy)
{
	// レイ情報設定
	RayInfo rayInfo = {};
	rayInfo.m_pos = m_position;
	rayInfo.m_pos.y += 0.6f;
	rayInfo.m_dir = enemy->GetMatrix().GetTranslation() - m_position;
	rayInfo.m_dir.Normalize();
	rayInfo.m_maxRange = m_searchRange;

	// 全Objectと判定
	KdRayResult finalRayResult = {};
	std::shared_ptr<GameObject> hitObject = nullptr;
	for (auto& object : OBJ_MAGER.GetObjectList())
	{
		if (object.get() == this)
			continue;

		// 自身がAlliesか確認 -> 無視する戦車TAG設定
		UINT ignoreTag = (m_tag & OBJECT_TAG::TAG_Allies)
			? OBJECT_TAG::TAG_Allies | OBJECT_TAG::TAG_Player
			: OBJECT_TAG::TAG_Enemy;

		// 無視するTAG
		if (object->GetTag() & (OBJECT_TAG::TAG_None | OBJECT_TAG::TAG_TreeObject | ignoreTag))
			continue;

		if (object->GetCollisionComponent() == nullptr)
			continue;

		// より近いObjectが無いか確認
		KdRayResult tmpResult = {};
		if (object->GetCollisionComponent()->HitCheckByRay(rayInfo, tmpResult))
		{
			if (tmpResult.m_distance < finalRayResult.m_distance)
			{
				finalRayResult = tmpResult;
				hitObject = object;
			}
		}
	}

	DRAW_MAGER.AddDebugLine(rayInfo.m_pos, finalRayResult.m_hitPos, kRedColor);

	if (hitObject == nullptr)
		return false;

	// 目標と一致
	if (hitObject == enemy)
		return true;

	return false;
}

//-----------------------------------------------------------------------------
// Name: SearchEnemy()
// Desc: 敵の索敵
//-----------------------------------------------------------------------------
void AutomaticTank::SearchEnemy()
{
	//--------------------------------------------------
	// 関数内関数でTAGの確認
	//--------------------------------------------------
	struct LocalFunc {
		// TAG確認
		static bool CheckTargetTag(const UINT myTag, const UINT targetTag) {
			// 自身が味方戦車か確認
			return (myTag & OBJECT_TAG::TAG_Allies)
				// 味方の場合...目標はEnemy
				? (targetTag & OBJECT_TAG::TAG_Enemy)
				// 敵の場合  ...目標はPlayerかAllies
				: targetTag & (OBJECT_TAG::TAG_Player | OBJECT_TAG::TAG_Allies);
		}
	};

	//--------------------------------------------------
	// 索敵
	//--------------------------------------------------

	for (auto& tank : OBJ_MAGER.GetTankList())
	{
		if (!LocalFunc::CheckTargetTag(m_tag, tank->GetTag()))
			continue;

		if (!tank->IsAlive())
			continue;

		// 目標との距離を算出 ※修正.一番近い距離を算出したほうがいい
		float distance = KdVector3::GetDistance(m_position, tank->GetMatrix().GetTranslation());
		if (distance <= m_searchRange)
		{
			if (CheckObstacle(tank))
				ChangeCombat(tank);
		}
	}
}

//-----------------------------------------------------------------------------
// Name: ChangeMission()
// Desc: 状態遷移.任務状態
//-----------------------------------------------------------------------------
bool AutomaticTank::ChangeMission()
{
	m_spState = std::make_shared<StateMission>();
	if (m_spState)
	{
		m_spState->Initialize(*this);
		m_onceRotFlg = false;
		return true;
	}
	return false;
}

//-----------------------------------------------------------------------------
// Name: ChangeWait()
// Desc: 待機状態に遷移
//-----------------------------------------------------------------------------
bool AutomaticTank::ChangeWait()
{
	m_spState = std::make_shared<StateWait>();
	if (m_spState)
	{
		m_spState->Initialize(*this);
		m_onceRotFlg = false;
		return true;
	}
	return false;
}

//-----------------------------------------------------------------------------
// Name: ChangeCombat()
// Desc: 状態遷移.戦闘状態
//-----------------------------------------------------------------------------
bool AutomaticTank::ChangeCombat(std::shared_ptr<Tank> enemy)
{
	if (enemy == nullptr)
		return false;

	m_spState = std::make_shared<StateCombat>();
	if (m_spState)
	{
		m_spState->Initialize(*this);
		m_enemyTarget = enemy;
		m_onceRotFlg = false;
		return true;
	}
	return false;
}



//=============================================================================
// 
// StateWait
// 
//=============================================================================

//-----------------------------------------------------------------------------
// Name: Initialize()
// Desc: 初期化
//-----------------------------------------------------------------------------
void AutomaticTank::StateWait::Initialize(AutomaticTank& owner)
{

}

//-----------------------------------------------------------------------------
// Name: Update()
// Desc: 更新
//-----------------------------------------------------------------------------
void AutomaticTank::StateWait::Update(AutomaticTank& owner)
{
	// 索敵
	owner.SearchEnemy();
	// 移動更新
	owner.UpdateMove(0.0f);
	// 衝突判定更新
	owner.UpdateCollision();
}



//=============================================================================
// 
// StateMission
// 
//=============================================================================

//-----------------------------------------------------------------------------
// Name: Initialize()
// Desc: 初期化
//-----------------------------------------------------------------------------
void AutomaticTank::StateMission::Initialize(AutomaticTank& owner)
{

}

//-----------------------------------------------------------------------------
// Name: Update()
// Desc: 更新
//-----------------------------------------------------------------------------
void AutomaticTank::StateMission::Update(AutomaticTank& owner)
{
	// 回転が終了しているか確認
	if (!owner.m_onceRotFlg) {
		if (owner.UpdateRotateBodyEx(owner.m_nextWayPoint - owner.m_position)) {	
			// 坂道などで回転がリセットされるので.車体がガタガタしないように
			owner.m_onceRotFlg = true;
		}
	}
	
	// 次の経由地点に車体が向いていたら前進
	const float virtualInputAxisY = owner.m_onceRotFlg ? 1.0f : 0.0f;
	owner.UpdateMove(virtualInputAxisY);

	// 次の経由地点を確認
	owner.CheckNextWayPoint();
	// 衝突判定更新
	owner.UpdateCollision();
	// 索敵
	owner.SearchEnemy();

	// 砲塔に車体のZ軸を送信
	if (owner.m_spTankParts)
		owner.m_spTankParts->SetTargetAxis(owner.m_worldMatrix.GetAxisZ());
}



//=============================================================================
// 
// StateCombat
// 
//=============================================================================

//-----------------------------------------------------------------------------
// Name: StateCombat()
// Desc: コンストラクタ
//-----------------------------------------------------------------------------
AutomaticTank::StateCombat::StateCombat()
{
}

//-----------------------------------------------------------------------------
// Name: Initialize()
// Desc: 初期化
//-----------------------------------------------------------------------------
void AutomaticTank::StateCombat::Initialize(AutomaticTank& owner)
{
}

//-----------------------------------------------------------------------------
// Name: Update()
// Desc: 更新
//-----------------------------------------------------------------------------
void AutomaticTank::StateCombat::Update(AutomaticTank& owner)
{
	// 敵が存在しなければ遷移.任務状態
	if (owner.m_enemyTarget == nullptr) {
		owner.ChangeMission();
		return;
	}

	// 敵への方向
	KdVector3 targetAxis = owner.m_enemyTarget->GetMatrix().GetTranslation() - owner.m_position;

	// 車体の回転更新
	owner.UpdateRotateBodyEx(targetAxis);
	// 障害物確認 -> なければ攻撃更新
	if (CheckObstacle(owner))
		owner.UpdateShot();

	// 移動更新
	owner.UpdateMove(0.0f);
	// 衝突判定更新
	owner.UpdateCollision();

	// 砲塔にカメラのZ軸を送信
	if (owner.m_spTankParts)
		owner.m_spTankParts->SetTargetAxis(targetAxis);

	// 敵を撃破したら任務に戻る
	if (!owner.m_enemyTarget->IsAlive())
		owner.ChangeMission();
}

//-----------------------------------------------------------------------------
// Name: CheckObstacle()
// Desc: 障害物の確認 ※障害物がない...true 障害物が存在...false
//-----------------------------------------------------------------------------
const bool AutomaticTank::StateCombat::CheckObstacle(AutomaticTank& owner)
{
	// 主砲のZ軸
	KdVector3 axis = owner.m_spTankParts->GetMainGun()->GetMatrix().GetAxisZ();

	// レイ情報設定
	RayInfo rayInfo = {};
	rayInfo.m_pos = owner.m_spTankParts->GetMainGun()->GetMatrix().GetTranslation();
	rayInfo.m_dir = axis;
	rayInfo.m_dir.Normalize();
	rayInfo.m_maxRange = KdVector3::GetDistance(owner.m_enemyTarget->GetMatrix().GetTranslation(), rayInfo.m_pos);

	// 全Objectと判定
	KdRayResult finalRayResult = {};
	std::shared_ptr<GameObject> hitObject = nullptr;
	for (auto& object : OBJ_MAGER.GetObjectList())
	{
		/*if (object.get() == owner.shared_from_this())
			continue;*/

		// 自身がAlliesか確認 -> 無視する戦車TAG設定
		UINT ignoreTag = (owner.m_tag & OBJECT_TAG::TAG_Allies)
			? OBJECT_TAG::TAG_Allies | OBJECT_TAG::TAG_Player
			: OBJECT_TAG::TAG_Enemy;

		// 無視するTAG
		if (object->GetTag() & (OBJECT_TAG::TAG_None | OBJECT_TAG::TAG_TreeObject | ignoreTag))
			continue;

		if (object->GetCollisionComponent() == nullptr)
			continue;

		// より近いObjectが無いか確認
		KdRayResult tmpResult = {};
		if (object->GetCollisionComponent()->HitCheckByRay(rayInfo, tmpResult))
		{
			if (tmpResult.m_distance < finalRayResult.m_distance)
			{
				finalRayResult = tmpResult;
				hitObject = object;
			}
		}
	}

	if (hitObject == nullptr)
		return false;

	// 目標と一致
	if (hitObject == owner.m_enemyTarget)
		return true;

	return false;
}



//=============================================================================
// 
// StateDead
// 
//=============================================================================

//-----------------------------------------------------------------------------
// Name: Initialize()
// Desc: 初期化
//-----------------------------------------------------------------------------
void AutomaticTank::StateDead::Initialize(AutomaticTank& owner)
{
	// 煙
	if (owner.m_deadSmkEffect)
		owner.m_deadSmkEffect->Play(owner.m_position);
}

//-----------------------------------------------------------------------------
// Name: Update()
// Desc: 更新
//-----------------------------------------------------------------------------
void AutomaticTank::StateDead::Update(AutomaticTank& owner)
{
}