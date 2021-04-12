//-----------------------------------------------------------------------------
// File: TitleTank.cpp
//
// Edit: 2021/03/19 野筒隼輔
//-----------------------------------------------------------------------------
#include "TitleTank.h"
#include "Application/Game/Weapon/TankParts.h"

//-----------------------------------------------------------------------------
// Name: Deserialize()
// Desc: 初期化
//-----------------------------------------------------------------------------
void TitleTank::Deserialize(const json11::Json& json_object)
{
	Tank::Deserialize(json_object);

	// キャタピラに送信する移動量のダミー
	m_movePow = 0.06f;
	// 砲塔の回転OFF
	m_spTankParts->SetTurretRot(false);
}

//-----------------------------------------------------------------------------
// Name: UpdateByState()
// Desc: 状態ごとの更新
//-----------------------------------------------------------------------------
void TitleTank::UpdateByState()
{
	//--------------------------------------------------
	// 前進
	//--------------------------------------------------
	{
		// 移動行列作成.合成
		KdMatrix transMat = {};
		transMat.CreateTranslation(0.0f, 0.0f, 0.08f);
		m_worldMatrix = transMat * m_worldMatrix;

		// 移動量
		m_force.x = m_worldMatrix.GetTranslation().x - m_prevPosition.x;
		m_force.z = m_worldMatrix.GetTranslation().z - m_prevPosition.z;

		// 重力による落下
		constexpr float gravity = 0.006f;
		m_force.y -= gravity;

		// 座標確定
		m_position += m_force;
	}

	// 移動時のEffect更新
	UpdateRunEffect(Math::Vector2(0.0f, 1.0f));
	// 衝突判定更新
	UpdateCollision();
}