//-----------------------------------------------------------------------------
// File: AerialBomb.h
//
// Edit: 2021/03/07 野筒隼輔
//-----------------------------------------------------------------------------
#pragma once
#include "Application/Game/GameObject.h"

//-----------------------------------------------------------------------------
// Name: class AerialBomb
// Desc: 航空爆弾クラス.爆撃機から投下(範囲攻撃)
//-----------------------------------------------------------------------------
class AerialBomb : public GameObject
{
public:

	// コンストラクタ
	AerialBomb();
	// 初期化
	void Deserialize(const json11::Json& json_object) override;
	// 更新
	void Update() override;
	// 発生(爆撃)
	void Emit(const KdVector3 pos);

private:
	std::shared_ptr<SoundInstance3D>	m_spExpSE;		// 爆発の効果音
	KdVector3							m_prevPosition;	// 1フレーム前の座標
	KdVector3							m_axis;			// 移動する軸
	float								m_speed;		// 移動速度
	float								m_gravity;		// 重力

private:
	void UpdateCollision();
	void Landing();
	void Explosion();
};