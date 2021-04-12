//-----------------------------------------------------------------------------
// File: PlayerTank.cpp
//
// Edit: 2021/02/26 野筒隼輔
//-----------------------------------------------------------------------------
#include "PlayerTank.h"
#include "PlayerInterface.h"
#include "Application/Game/AI/Character/CheckObstacleBall.h"
#include "Application/Game/Scene/Scene.h"
#include "Application/Game/Weapon/TankParts.h"

#include "Application/Component/CameraComponent.h"
#include "Application/Component/InputComponent.h"

#include "Application/Manager/CameraManager.h"
#include "Application/Manager/DrawManager.h"
#include "Application/Manager/DebugManager.h"
#include "Application/Manager/ObjectManager.h"

//-----------------------------------------------------------------------------
// Name: PlayerTank()
// Desc: コンストラクタ
//-----------------------------------------------------------------------------
PlayerTank::PlayerTank()
	: m_spState(nullptr)
	, m_spPlayerInterface(nullptr)
	, m_checkBall(nullptr)
	, m_isAim(false)
	, m_onceCameraSet(false)
	, m_aimSpeed(0.06f)
{
}

//-----------------------------------------------------------------------------
// Name: Deserialize()
// Desc: 初期化
//-----------------------------------------------------------------------------
void PlayerTank::Deserialize(const json11::Json& json_object)
{
	// 共通の初期化
	Tank::Deserialize(json_object);

	// 状態.3人称状態から
	m_spState = std::make_shared<StateMove>();
	if (m_spState) { m_spState->Initialize(*this); }

	// UI作成
	m_spPlayerInterface = std::make_shared<PlayerInterface>(*this);
	if (m_spPlayerInterface) { m_spPlayerInterface->Deserialize(); }

	// コントローラ作成
	m_spInputComponent = std::make_shared<TankInputComponent>(*this);

	// カメラをターゲットに設定
	if (m_spCameraComponent) { CAMERA_MAGER.SetTargetCamera(m_spCameraComponent); }
}

//-----------------------------------------------------------------------------
// Name: Update()
// Desc: 更新 編集カメラ使用中は更新を停止
//-----------------------------------------------------------------------------
void PlayerTank::Update()
{
	if (SCENE.IsEdiorMode()) { return; }
	if (CAMERA_MAGER.IsAnimation()) { return; }
	Tank::Update();

	if(false) {
		// ライトの向き
		DirectX::XMFLOAT3 lightVec(0.5f, -1.0f, 0.5f);
		// 影が落ちる地面
		DirectX::XMFLOAT4 planeVec(0.0f, 1.0f, 0.0f, 0.0f);
		// 影行列作成
		DirectX::XMMATRIX shadowMat = DirectX::XMMatrixShadow(DirectX::XMLoadFloat4(&planeVec), DirectX::XMLoadFloat3(&lightVec));
	}
}

//-----------------------------------------------------------------------------
// Name: DrawSprite()
// Desc: 2D描画
//-----------------------------------------------------------------------------
void PlayerTank::DrawSprite()
{
	// UI
	if (m_spPlayerInterface) { m_spPlayerInterface->DrawSprite(); }
}

//-----------------------------------------------------------------------------
// Name: UpdateByState()
// Desc: 状態ごとの更新 ※基底クラスの純粋仮想関数をオーバーロード
//-----------------------------------------------------------------------------
void PlayerTank::UpdateByState()
{
	// 初回のカメラ固定
	if (!m_onceCameraSet) { OnceCameraSetting(); }

	// カメラ更新
	if (m_onceCameraSet) { UpdateCamera(); }
	// 状態ごとの更新
	if (m_spState) { m_spState->Update(*this); }
	// UI
	if (m_spPlayerInterface) { m_spPlayerInterface->Update(); }
}

//-----------------------------------------------------------------------------
// Name: Destroy()
// Desc: 死亡 ※修正.Tankに記述してさらに継承する
//-----------------------------------------------------------------------------
void PlayerTank::Destroy()
{
	// エフェクト
	KdVector3 effectPos = m_position;
	effectPos.y -= 0.4f;
	if (m_deadExpEffect) { m_deadExpEffect->Play(effectPos); }

	// 効果音
	AUDIO.Play3D("Data/Audio/SE/Explosion.wav", m_position);

	// 死体撃ちで消滅
	if (!m_isAlive) {
		Exclusion();
		if (m_deadSmkEffect) { m_deadSmkEffect->Stop(); }
		return;
	}

	// 状態.死亡へ
	m_spState = std::make_shared<StateDead>();
	if (m_spState) { m_spState->Initialize(*this); }

	// 部品が壊れる
	if (m_spTankParts) { m_spTankParts->Crash(); }
	// キャタピラの音を消す
	if (m_spRunBGM) { m_spRunBGM->Stop(); }

	GameObject::Destroy();
}

//-----------------------------------------------------------------------------
// Name: OnceCameraSetting()
// Desc: ゲームが始まった際にカメラが動かないように数フレーム固定
//-----------------------------------------------------------------------------
void PlayerTank::OnceCameraSetting()
{
	// 固定するフレーム数
	constexpr std::uint8_t count_max = 30;
	// 指定フレーム数カウント.その間カメラ固定
	static std::uint8_t count = 0;
	count++;
	if (count >= count_max) {
		count = 0;
		m_onceCameraSet = true;
	}
}

//-----------------------------------------------------------------------------
// Name: UpdateCamera()
// Desc: カメラ更新 ※操作 修正.関数内にマジックナンバーが多数存在
//-----------------------------------------------------------------------------
void PlayerTank::UpdateCamera()
{
	if ((m_spCameraComponent == nullptr) || (m_spInputComponent == nullptr)) { return; }

	// 入力値所得
	const Math::Vector2& inputMove = m_spInputComponent->GetAxis(Input::Axes::R);
	if (inputMove.LengthSquared() == 0.0f) {
		DRAW_MAGER.SetBlurValue(Math::Vector2::Zero);
		return;
	}

	// 最終的な値
	Math::Vector2 rotate_vector = inputMove;

	// 移動時のぼかし
	const float testNum = 30.0f;
	if ((rotate_vector.x >= testNum) || (rotate_vector.y >= testNum) ||
		(rotate_vector.x <= -testNum) || (rotate_vector.y <= -testNum)) {
		DRAW_MAGER.SetBlurValue(rotate_vector * 0.04f);
	}
	else {
		DRAW_MAGER.SetBlurValue(Math::Vector2::Zero);
	}

	// エイム速度
	rotate_vector *= m_aimSpeed;

	// 回転させる値確定
	float rotateX = rotate_vector.x * KdToRadians;
	float rotateY = rotate_vector.y * KdToRadians * 0.6f;// 縦のエイム速度は少し落とす

	// カメラのView行列のZ軸(見ている方向)設定
	float cameraDot = KdVector3::Dot(m_spCameraComponent->GetCameraMatrix().GetAxisZ(), KdVector3::Up());

	// Yの角度制限
	if (cameraDot - rotateY <= -4.0f * KdToRadians) { rotateY = -4.0f; }
	if (cameraDot - rotateY >= 20.0f * KdToRadians) { rotateY =  0.0f; }

	// Offset行列作成
	KdMatrix rotateMat = {};
	rotateMat.CreateRotationX(rotateY);
	m_spCameraComponent->OffsetMatrix() = rotateMat * m_spCameraComponent->OffsetMatrix();
	m_spCameraComponent->OffsetMatrix().RotateY(rotateX);
}

//-----------------------------------------------------------------------------
// Name: UpdateSearchEnemy()
// Desc: 敵の索敵を行う
//-----------------------------------------------------------------------------
void PlayerTank::UpdateSearchEnemy()
{
	for (auto& tank : OBJ_MAGER.GetTankList()) {
		if (!tank->IsAlive()) { continue; }
		if (tank->GetTag() & OBJECT_TAG::TAG_Enemy) {
			if (m_checkBall) { continue; }
			// 確認用のObjectインスタンス化.Objectコンテナに追加
			m_checkBall = std::make_shared<CheckObstacleBall>(*this);
			if (m_checkBall == nullptr) { continue; }
			m_checkBall->Deserialize(KdResFac.GetJson("Data/Text/Object/Weapon/CannonBall.json"));
			OBJ_MAGER.AddObject(m_checkBall);
			// 敵への方向を設定
			KdVector3 axis;
			axis = tank->GetMatrix().GetTranslation() - m_position;
			axis.Normalize();
			m_checkBall->SetAxis(axis);
		}
	}
}



//=============================================================================
// 
// StateMove
// 
//=============================================================================

//-----------------------------------------------------------------------------
// Name: Initialize()
// Desc: 初期化
//-----------------------------------------------------------------------------
void PlayerTank::StateMove::Initialize(PlayerTank& owner)
{
	KdVector3 body_axisZ = KdVector3(0.0f, 0.0f, 1.0f);
	KdVector3 camera_axisZ = owner.m_spCameraComponent->GetCameraMatrix().GetAxisZ();
	const float camera_angleY = KdVector3::GetAngle(Math::Vector2(body_axisZ.x, body_axisZ.z), Math::Vector2(camera_axisZ.x, camera_axisZ.z));
	// カメラ設定
	owner.m_spCameraComponent->SetFovAngle(40.0f * KdToRadians);
	owner.m_spCameraComponent->OffsetMatrix().CreateRotationX(KdVector3::Dot(camera_axisZ, KdVector3::Up()) * -1);
	owner.m_spCameraComponent->OffsetMatrix().Move(0.0f, 3.0f, -10.0f);
	owner.m_spCameraComponent->OffsetMatrix().RotateY(camera_angleY * -1);

	// エイム速度設定
	owner.m_isAim = false;
	owner.m_aimSpeed = 0.06f;
}

//-----------------------------------------------------------------------------
// Name: Update()
// Desc: 更新
//-----------------------------------------------------------------------------
void PlayerTank::StateMove::Update(PlayerTank& owner)
{
	// 射撃更新
	if (owner.m_spInputComponent->GetButton(Input::L1) & Input::ButtonState::ENTER) {
		owner.UpdateShot();
	}

	// 移動更新
	owner.UpdateMove(owner.m_spInputComponent->GetAxis(Input::Axes::L).y);
	// 車体の回転更新
	owner.UpdateRotateBody(owner.m_spInputComponent->GetAxis(Input::Axes::L).x);
	// 移動時のエフェクト更新
	owner.UpdateRunEffect(owner.m_spInputComponent->GetAxis(Input::Axes::L));
	// 移動時のBGM更新
	owner.UpdateRunBGM();
	// 敵の索敵更新
	owner.UpdateSearchEnemy();

	// 衝突判定更新
	owner.UpdateCollision();

	// 遷移.1人称
	if (owner.m_spInputComponent->GetButton(Input::R1) & Input::ButtonState::ENTER) {
		owner.m_spState = std::make_shared<StateAim>();
		owner.m_spState->Initialize(owner);
	}
}



//=============================================================================
// 
// StateAim
// 
//=============================================================================

//-----------------------------------------------------------------------------
// Name: Initialize()
// Desc: 初期化
//-----------------------------------------------------------------------------
void PlayerTank::StateAim::Initialize(PlayerTank& owner)
{
	KdVector3 body_axisZ = KdVector3(0.0f, 0.0f, 1.0f);
	KdVector3 camera_axisZ = owner.m_spCameraComponent->GetCameraMatrix().GetAxisZ();
	const float camera_angleY = KdVector3::GetAngle(Math::Vector2(body_axisZ.x, body_axisZ.z), Math::Vector2(camera_axisZ.x, camera_axisZ.z));
	// カメラ設定
	owner.m_spCameraComponent->SetFovAngle(16.0f * KdToRadians);
	owner.m_spCameraComponent->OffsetMatrix().CreateRotationX(KdVector3::Dot(camera_axisZ, KdVector3::Up()) * -1);
	owner.m_spCameraComponent->OffsetMatrix().Move(0.0f, 2.2f, 5.0f);
	owner.m_spCameraComponent->OffsetMatrix().RotateY(camera_angleY * -1);

	// エイム速度設定
	owner.m_isAim = true;
	owner.m_aimSpeed = 0.03f;
}

//-----------------------------------------------------------------------------
// Name: Update()
// Desc: 更新
//-----------------------------------------------------------------------------
void PlayerTank::StateAim::Update(PlayerTank& owner)
{
	// 射撃更新
	if (owner.m_spInputComponent->GetButton(Input::L1) & Input::ButtonState::ENTER) {
		owner.UpdateShot();
	}

	// 移動更新
	owner.UpdateMove(owner.m_spInputComponent->GetAxis(Input::Axes::L).y);
	// 車体の回転更新
	owner.UpdateRotateBody(owner.m_spInputComponent->GetAxis(Input::Axes::L).x);
	// 移動時のBGM更新
	owner.UpdateRunBGM();

	// 衝突判定更新
	owner.UpdateCollision();

	// 遷移.3人称
	if (owner.m_spInputComponent->GetButton(Input::R1) & Input::ButtonState::EXIT) {
		owner.m_spState = std::make_shared<StateMove>();
		owner.m_spState->Initialize(owner);
	}
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
void PlayerTank::StateDead::Initialize(PlayerTank& owner)
{
}

//-----------------------------------------------------------------------------
// Name: Update()
// Desc: 更新
//-----------------------------------------------------------------------------
void PlayerTank::StateDead::Update(PlayerTank& owner)
{
}