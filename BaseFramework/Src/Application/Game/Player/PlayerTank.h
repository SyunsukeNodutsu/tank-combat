//-----------------------------------------------------------------------------
// File: PlayerTank.h
//
// Edit: 2021/02/26 野筒隼輔
//-----------------------------------------------------------------------------
#pragma once
#include "Application/Game/Weapon/Tank.h"

//--------------------------------------------------
// 前方宣言
//--------------------------------------------------
class PlayerInterface;
class CheckObstacleBall;

//-----------------------------------------------------------------------------
// Name: class PlayerTank
// Desc: 操作戦車クラス
//-----------------------------------------------------------------------------
class PlayerTank : public Tank
{
	class StateBase;
public:

	// コンストラクタ
	PlayerTank();
	// 初期化
	void Deserialize(const json11::Json& json_object) override;
	// 更新
	void Update() override;
	// 2D描画
	void DrawSprite() override;
	// 状態ごとの更新 ※TankクラスのUpdate内で呼ばれる
	void UpdateByState() override;
	// 死亡
	void Destroy() override;

	//--------------------------------------------------
	// 所得
	//--------------------------------------------------
	// 部品所得
	const std::shared_ptr<TankParts> GetTankParts() const { return m_spTankParts; }
	// リロードのカウント所得
	const int GetReloadCount() const { return m_reloadCount; }
	// 1人称かどうか所得
	const bool IsAim() const { return m_isAim; }

private:
	std::shared_ptr<StateBase>			m_spState;			// 状態
	std::shared_ptr<PlayerInterface>	m_spPlayerInterface;// UI
	std::shared_ptr<CheckObstacleBall>	m_checkBall;		// 
	bool								m_isAim;			// 1人称かどうか
	bool								m_onceCameraSet;	// 初回のカメラセット
	float								m_aimSpeed;			// エイム速度

private:
	void OnceCameraSetting();
	void UpdateCamera();
	void UpdateSearchEnemy();

private:
	//--------------------------------------------------
	// 状態管理 ステートマシン
	//--------------------------------------------------

	// 基底抽象ステート
	class StateBase
	{
	public:
		virtual void Initialize(PlayerTank& owner) = 0;
		virtual void Update(PlayerTank& owner) = 0;
	};

	// 3人称 ※修正.いい命名がないか考える
	class StateMove : public StateBase
	{
	public:
		void Initialize(PlayerTank& owner);
		void Update(PlayerTank& owner);
	};

	// 1人称
	class StateAim : public StateBase
	{
	public:
		void Initialize(PlayerTank& owner);
		void Update(PlayerTank& owner);
	};

	// 死亡
	class StateDead : public StateBase
	{
	public:
		void Initialize(PlayerTank& owner);
		void Update(PlayerTank& owner);
	};
};