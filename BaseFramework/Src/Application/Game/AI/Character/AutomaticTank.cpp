//-----------------------------------------------------------------------------
// File: AutomaticTank.h
//
// Edit: 2021/03/19 野筒隼輔
//-----------------------------------------------------------------------------
#include "AutomaticTank.h"
#include "Application/Game/Weapon/TankParts.h"
#include "Application/Component/CameraComponent.h"
#include "Application/Manager/ClearManager.h"

//-----------------------------------------------------------------------------
// Name: AutomaticTank()
// Desc: コンストラクタ
//-----------------------------------------------------------------------------
AutomaticTank::AutomaticTank()
	: m_spState(nullptr)
{
}

//-----------------------------------------------------------------------------
// Name: Deserialize()
// Desc: 初期化
//-----------------------------------------------------------------------------
void AutomaticTank::Deserialize(const json11::Json& json_object)
{
	Tank::Deserialize(json_object);

	// 状態.ミッション状態から
	m_spState = std::make_shared<StateMission>();
	m_spState->Initialize(*this);
}

//-----------------------------------------------------------------------------
// Name: UpdateByState()
// Desc: 状態ごとの更新
//-----------------------------------------------------------------------------
void AutomaticTank::UpdateByState()
{
	// 状態ごとの更新
	if (m_spState) { m_spState->Update(*this); }
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
	m_spState->Initialize(*this);

	// 部品が壊れる
	m_spTankParts->Crash();

	// 撃破通知
	if (m_tag & OBJECT_TAG::TAG_Enemy) { CLEAR_MAGER.EnemyDefeat(); }

	GameObject::Destroy();
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
	// 移動更新
	owner.UpdateMove(1.0f);
	// 車体の回転更新
	owner.UpdateRotateBody(1.0f);

	// 衝突判定更新
	owner.UpdateCollision();
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
	owner.m_deadSmkEffect->Play(owner.m_position);
}

//-----------------------------------------------------------------------------
// Name: Update()
// Desc: 更新
//-----------------------------------------------------------------------------
void AutomaticTank::StateDead::Update(AutomaticTank& owner)
{
}