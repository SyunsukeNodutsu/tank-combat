//-----------------------------------------------------------------------------
// File: Tank.h
//
// Edit: 2021/03/17 野筒隼輔
//-----------------------------------------------------------------------------
#pragma once
#include "Application/Game/GameObject.h"

//--------------------------------------------------
// 前方宣言
//--------------------------------------------------
class TankParts;

//-----------------------------------------------------------------------------
// Name: class Tank
// Desc: 戦車クラス
//-----------------------------------------------------------------------------
class Tank : public GameObject
{
public:

	// コンストラクタ
	Tank();
	// 初期化
	virtual void Deserialize(const json11::Json& json_object) override;
	// 更新
	virtual void Update() override;
	// 状態ごとの更新 ※純粋仮想関数.派生先に任せる
	virtual void UpdateByState() = 0;
	// 描画
	virtual void Draw() override;
	// シャドウマップ描画
	virtual void DrawShadowMap() override;

	//--------------------------------------------------
	// 所得
	//--------------------------------------------------
	// 移動速度所得
	const float GetMoveSpeed() const { return m_movePow; }
	// 回転速度所得
	const float GetRotateSpeed() const { return m_rotatePow; }
	// リロードに必要なカウント数所得
	const int GetReloadCountMax() const { return m_reloadCountMax; }
	// 敵に発見されているか所得
	const bool IsDiscovery() const { return m_isDiscovery; }

	// 攻撃回数所得
	const int GetAttackNum() { return m_AttackNum; }
	// 命中回数所得
	const int GetAttackHitNum() { return m_AttackHitNum; }
	// 撃破回数所得
	const int GetDefeatsNum() { return m_DefeatsNum; }

	//--------------------------------------------------
	// 設定
	//--------------------------------------------------
	// 航空機爆弾の着弾通知
	void NoticeAerialBombLanding(const KdVector3 position);
	// 発見通知
	void Discovery() { m_isDiscovery = true; }
	// 見失う
	void LoseSight() { m_isDiscovery = false; }

	// 命中回数を加算
	void AttackHit() { m_AttackHitNum++; }
	// 撃破回数を加算
	void EnemyDefeats() { m_DefeatsNum++; }

protected:
	std::shared_ptr<TankParts>		m_spTankParts;		// 戦車の部品
	KdVector3						m_position;			// 座標
	KdVector3						m_prevPosition;		// 1フレーム前の座標
	KdVector3						m_force;			// 移動量
	float							m_movePow;			// 最終的な移動速度 ※加減速に対応
	float							m_rotatePow;		// 最終的な回転速度 ※加減速に対応
	int								m_reloadCount;		// 砲弾のリロードをカウント
	int								m_reloadCountMax;	// 砲弾のリロードに必要なカウント数
	bool							m_isDiscovery;		// 敵に発見されているか

	// 戦果用 ※修正.クラス化したほうがきれい
	int m_AttackNum		= 0; // 攻撃回数
	int m_AttackHitNum	= 0; // 命中回数
	int m_DefeatsNum	= 0; // 撃破回数

	// エフェクト ※コンポーネントにしたい
	std::shared_ptr<EffekseerEffectInstance>	m_runEffect;	// 走行時の煙
	std::shared_ptr<EffekseerEffectInstance>	m_deadSmkEffect;// 行動不能時の煙
	std::shared_ptr<EffekseerEffectInstance>	m_deadExpEffect;// 行動不能時の爆発
	std::shared_ptr<EffekseerEffectInstance>	m_fireEffect;	// 主砲発射時
	// 効果音 ※コンポーネントにしたい
	std::shared_ptr<SoundInstance3D>			m_spFireSE;		// 発射時の効果音
	std::shared_ptr<SoundInstance3D>			m_spRunBGM;		// 移動時のBGM

protected:

	// 移動更新
	void UpdateMove(const float input_axisY);
	// 車体の回転更新
	void UpdateRotateBody(const float input_axisX);
	// エフェクト更新
	void UpdateRunEffect(const Math::Vector2 input_move);
	// 衝突判定更新
	void UpdateCollision();
	// 射撃(攻撃)更新 ※特定のトリガーで呼ばれる.Enemyの場合は引数に指定
	void UpdateShot(const KdVector3 enemy_ver_axiz = KdVector3(0.0f, 0.0f, 0.0f));
	// 移動時のBGM更新
	void UpdateRunBGM();

private:
	void ComponentInitialSetting();
	void ResourceInitialSetting();
	KdRayResult CheckGround(float& dst_distance, KdVector3 ray_pos);
	void CheckBump();
	void TiltBody(std::array<KdVector3, 4>& ray_pos, float& dst_distance);
};