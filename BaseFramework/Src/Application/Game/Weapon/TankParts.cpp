//-----------------------------------------------------------------------------
// File: TankParts.cpp
//
// Edit: 2021/03/28 野筒隼輔
//-----------------------------------------------------------------------------
#include "TankParts.h"
#include "Application/Game/GameObject.h"
#include "Application/Game/Weapon/Tank.h"
#include "Application/Component/CameraComponent.h"
#include "Application/Component/ModelComponent.h"

//-----------------------------------------------------------------------------
// Name: TankParts()
// Desc: コンストラクタ オーナーの設定
//-----------------------------------------------------------------------------
TankParts::TankParts(Tank& owner)
	: m_owner(owner)
	, m_mainGun(nullptr)
	, m_mainGunLocalMat()
	, m_mainGunRotNum(0.0f)
	, m_turret(nullptr)
	, m_turretLocalMat()
	, m_targetAxis(KdVector3(0.0f, 0.0f, 0.0f))
	, m_turretRotNum(0.0f)
	, m_turretRotSpeed(0.02f)
	, m_isRot(true)
	, m_leftTrack(nullptr)
	, m_leftTrackLocalMat()
	, m_rightTrack(nullptr)
	, m_rightTrackLocalMat()
{
}

//-----------------------------------------------------------------------------
// Name: Deserialize()
// Desc: 初期化
//-----------------------------------------------------------------------------
void TankParts::Deserialize(const json11::Json& json_object)
{
	if (json_object.is_null()) {
		assert(0 && "[Deserialize] : error json objsct");
		return;
	}

	// 各パーツ初期化
	bool result = true;
	result = SettingMainGun(json_object["MainGun"].object_items());
	result = SettingTurret(json_object["Turret"].object_items());
	result = SettingTrack(json_object["Track"].object_items());
}

//-----------------------------------------------------------------------------
// Name: Update()
// Desc: 更新
//-----------------------------------------------------------------------------
void TankParts::Update()
{
	UpdateMainGun();
	UpdateTurret();
	UpdateTrack();
}

//-----------------------------------------------------------------------------
// Name: Draw()
// Desc: 描画
//-----------------------------------------------------------------------------
void TankParts::Draw()
{
	if (m_mainGun) { m_mainGun->Draw(); }
	if (m_turret) { m_turret->Draw(); }
	if (m_leftTrack) { m_leftTrack->Draw(); }
	if (m_rightTrack) { m_rightTrack->Draw(); }
}

//-----------------------------------------------------------------------------
// Name: DrawShadowMap()
// Desc: シャドウマップ描画
//-----------------------------------------------------------------------------
void TankParts::DrawShadowMap()
{
	if (m_mainGun) { m_mainGun->DrawShadowMap(); }
	if (m_turret) { m_turret->DrawShadowMap(); }
	if (m_leftTrack) { m_leftTrack->DrawShadowMap(); }
	if (m_rightTrack) { m_rightTrack->DrawShadowMap(); }
}

//-----------------------------------------------------------------------------
// Name: Crash()
// Desc: 死亡時に壊れた演出
//-----------------------------------------------------------------------------
void TankParts::Crash()
{
	if (m_mainGun == nullptr) { return; }
	if (!m_leftTrack || !m_rightTrack) { return; }

	// 主砲も
	KdMatrix rotMat = {};
	rotMat.CreateRotationX(10.0f * KdToRadians);
	m_mainGunLocalMat = rotMat * m_mainGunLocalMat;
	m_mainGun->SetMatrix(m_mainGunLocalMat * m_owner.GetMatrix());

	// 砲塔の回転値を0
	m_turretRotNum = 0.0f;

	// キャタピラのUVスクロールOFF
	m_leftTrack->GetModelComponent()->SetUVScroll(false);
	m_rightTrack->GetModelComponent()->SetUVScroll(false);
}

//-----------------------------------------------------------------------------
// Name: SettingMainGun()
// Desc: 主砲の初期化
//-----------------------------------------------------------------------------
const bool TankParts::SettingMainGun(const json11::Json& json_object)
{
	if (json_object.is_null()) {
		assert(0 && "[SettingMainGun] : error json objsct");
		return false;
	}

	// 生成.初期化
	m_mainGun = std::make_shared<GameObject>();
	m_mainGun->Deserialize(json_object);

	// ローカル座標設定
	if (!json_object["LocalPosition"].is_null()) {
		auto& localPos = json_object["LocalPosition"].array_items();
		m_mainGunLocalMat.CreateTranslation(KdVector3(
			static_cast<float>(localPos[0].number_value()),
			static_cast<float>(localPos[1].number_value()),
			static_cast<float>(localPos[2].number_value()))
		);
	}

	return true;
}

//-----------------------------------------------------------------------------
// Name: SettingTurret()
// Desc: 砲塔の初期化
//-----------------------------------------------------------------------------
const bool TankParts::SettingTurret(const json11::Json& json_object)
{
	if (json_object.is_null()) {
		assert(0 && "[SettingTurret] : error json objsct");
		return false;
	}

	// 生成.初期化
	m_turret = std::make_shared<GameObject>();
	m_turret->Deserialize(json_object);

	// ローカル座標設定
	if (!json_object["LocalPosition"].is_null()) {
		auto& localPos = json_object["LocalPosition"].array_items();
		m_turretLocalMat.CreateTranslation(KdVector3(
			static_cast<float>(localPos[0].number_value()),
			static_cast<float>(localPos[1].number_value()),
			static_cast<float>(localPos[2].number_value()))
		);
	}

	// 砲塔の回転速度
	if (!json_object["TurretRotSpeed"].is_null()) {
		m_turretRotSpeed = static_cast<float>(json_object["TurretRotSpeed"].number_value());
	}

	return true;
}

//-----------------------------------------------------------------------------
// Name: SettingTrack()
// Desc: キャタピラの初期化
//-----------------------------------------------------------------------------
const bool TankParts::SettingTrack(const json11::Json& json_object)
{
	if (json_object.is_null()) {
		assert(0 && "[SettingTrack] : error json objsct");
		return false;
	}

	//--------------------------------------------------
	// 左のキャタピラ
	//--------------------------------------------------
	
	// 生成.初期化
	m_leftTrack = std::make_shared<GameObject>();
	m_leftTrack->Deserialize(json_object);
	m_leftTrack->GetModelComponent()->SetUVScroll(true);

	// ローカル座標設定
	if (!json_object["LocalPosition"].is_null()) {
		auto& localPos = json_object["LocalPosition"].array_items();
		m_leftTrackLocalMat.CreateTranslation(KdVector3(
			static_cast<float>(localPos[0].number_value()) * -1,
			static_cast<float>(localPos[1].number_value()),
			static_cast<float>(localPos[2].number_value()))
		);
	}

	//--------------------------------------------------
	// 右のキャタピラ
	//--------------------------------------------------
	
	// 生成.初期化
	m_rightTrack = std::make_shared<GameObject>();
	m_rightTrack->Deserialize(json_object);
	m_rightTrack->GetModelComponent()->SetUVScroll(true);

	// ローカル座標設定
	if (!json_object["LocalPosition"].is_null()) {
		auto& localPos = json_object["LocalPosition"].array_items();
		m_rightTrackLocalMat.CreateTranslation(KdVector3(
			static_cast<float>(localPos[0].number_value()),
			static_cast<float>(localPos[1].number_value()),
			static_cast<float>(localPos[2].number_value()))
		);
	}

	return true;
}

//-----------------------------------------------------------------------------
// Name: UpdateMainGun()
// Desc: 主砲更新
//-----------------------------------------------------------------------------
void TankParts::UpdateMainGun()
{
	if (m_mainGun == nullptr) { return; }
	if (!m_owner.IsAlive()) { return; }

	// カメラのZ軸所得
	KdVector3 camera_axizZ = m_owner.GetCameraComponent()->GetCameraMatrix().GetAxisZ();

	//--------------------------------------------------
	// 回転
	//--------------------------------------------------
	if (m_owner.GetCameraComponent() && m_isRot && (camera_axizZ.LengthSquared() != 0))
	{
		// 砲塔のZ軸
		KdVector3 mainGunAxisZ = m_mainGun->GetMatrix().GetAxisZ();
		mainGunAxisZ.Normalize();

		// 砲塔のY軸
		KdVector3 turretAxisY = m_turret->GetMatrix().GetAxisY();
		turretAxisY.Normalize();

		// カメラと主砲の角度
		float dot = KdVector3::Dot(camera_axizZ, turretAxisY) * -1;

		// 現在の主砲の角度
		float dot2 = KdVector3::Dot(mainGunAxisZ, turretAxisY) * -1;

		// 差分を算出
		float rotateRadian = dot - dot2;

		// 最大値以内に補正
		rotateRadian = std::clamp(rotateRadian, -0.02f, 0.02f);

		// 回転の値確定
		m_mainGunRotNum = rotateRadian;

		// 回転させる
		KdMatrix rotMat = {};
		rotMat.CreateRotationX(m_mainGunRotNum);
		m_mainGunLocalMat = rotMat * m_mainGunLocalMat;
		// 横の回転は砲塔と連動 ... m_turretRotNum
		m_mainGunLocalMat.RotateY(m_turretRotNum);
	}

	//--------------------------------------------------
	// ワールド行列に合成
	//--------------------------------------------------
	KdMatrix worldMat = m_owner.GetMatrix();
	m_mainGun->SetMatrix(m_mainGunLocalMat * worldMat);
}

//-----------------------------------------------------------------------------
// Name: UpdateTurret()
// Desc: 砲塔更新
//-----------------------------------------------------------------------------
void TankParts::UpdateTurret()
{
	if (m_turret == nullptr) { return; }
	if (!m_owner.IsAlive()) { return; }

	//--------------------------------------------------
	// 回転
	//--------------------------------------------------
	if (m_owner.GetCameraComponent() && m_isRot && (m_targetAxis.LengthSquared() != 0))
	{
		// 砲塔のZ軸
		KdVector3 nowDir = m_turret->GetMatrix().GetAxisZ();
		nowDir.Normalize();

		// 差分を算出
		float rotateRadian = atan2f(m_targetAxis.x, m_targetAxis.z) - atan2f(nowDir.x, nowDir.z);

		// 180度の際に補正
		if (rotateRadian > M_PI) { rotateRadian -= 2 * static_cast<float>(M_PI); }
		else if (rotateRadian < -M_PI) { rotateRadian += 2 * static_cast<float>(M_PI); }

		// 最大値以内に補正
		rotateRadian = std::clamp(rotateRadian, -m_turretRotSpeed, m_turretRotSpeed);

		// 回転の値確定
		m_turretRotNum = rotateRadian;

		// 回転完了
		m_turretLocalMat.RotateY(m_turretRotNum);
	}

	//--------------------------------------------------
	// ワールド行列に合成
	//--------------------------------------------------
	KdMatrix worldMat = m_owner.GetMatrix();
	m_turret->SetMatrix(m_turretLocalMat * worldMat);
}

//-----------------------------------------------------------------------------
// Name: UpdateTrack()
// Desc: キャタピラ更新
//-----------------------------------------------------------------------------
void TankParts::UpdateTrack()
{
	if (!m_leftTrack || !m_rightTrack) { return; }
	if (!m_owner.IsAlive()) { return; }

	//--------------------------------------------------
	// UVスクロール設定
	//--------------------------------------------------

	// 移動
	float trackMovePow = m_owner.GetMoveSpeed();
	if (trackMovePow >= 0.2f) { trackMovePow = 0.2f; }

	// 回転
	const float trackRotPow = m_owner.GetRotateSpeed() * 0.06f;
	const float raito = (trackRotPow == 0.0f) ? 0.4f : 0.4f;

	// done
	m_leftTrack->GetModelComponent()->SetUVMoveNum(Math::Vector2(0.0f, ((trackMovePow * raito) * -1) + (trackRotPow * 1)));
	m_rightTrack->GetModelComponent()->SetUVMoveNum(Math::Vector2(0.0f, ((trackMovePow * raito) * -1) + (trackRotPow * -1)));

	//--------------------------------------------------
	// ローカル行列設定
	//--------------------------------------------------

	// オーナーのワールド行列
	KdMatrix worldMat = m_owner.GetMatrix();

	m_leftTrack->SetMatrix(m_leftTrackLocalMat * worldMat);
	m_rightTrack->SetMatrix(m_rightTrackLocalMat * worldMat);
}