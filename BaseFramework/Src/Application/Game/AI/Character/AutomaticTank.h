//-----------------------------------------------------------------------------
// File: AutomaticTank.h
//
// Edit: 2021/05/14 野筒隼輔
//-----------------------------------------------------------------------------
#pragma once
#include "Application/Game/Weapon/Tank.h"

class CheckObstacleBall;

//-----------------------------------------------------------------------------
// Name: class AutomaticTank
// Desc: 自動戦車クラス ※修正.StateMachineの遷移を関数にする
//-----------------------------------------------------------------------------
class AutomaticTank : public Tank
{
	class StateBase;
public:

	// コンストラクタ
	AutomaticTank();
	// 初期化
	void Deserialize(const json11::Json& json_object) override;
	// 状態ごとの更新 ※TankクラスのUpdate内で呼ばれる
	void UpdateByState() override;
	// 死亡
	void Destroy() override;

	// 前進命令
	void AdvanceOrder(const KdVector3 position);
	// 次の経由地点を確認
	void CheckNextWayPoint();
	// 引数の軸に車体を回転させる
	const bool UpdateRotateBodyEx(const KdVector3 axis);
	// 索敵
	void SearchEnemy();

	// 任務状態に遷移
	bool ChangeMission();
	// 待機状態に遷移
	bool ChangeWait();
	// 戦闘状態に遷移
	bool ChangeCombat(std::shared_ptr<Tank> enemy);

private:
	std::shared_ptr<StateBase>				m_spState;			// 状態
	std::unordered_map<uint8_t, KdVector3>	m_wayPoint;			// 経由地点
	uint8_t									m_wayPointCount;	// 経由地点を何回経由したか
	KdVector3								m_nextWayPoint;		// 前進の目標座標
	float									m_searchRange;		// 索敵範囲 ※視覚の距離
	bool									m_onceRotFlg;		// 経由地点に車体が回転を終えたか
	std::shared_ptr<Tank>					m_enemyTarget;		// 攻撃目標 ※修正.各Stateに持たせて.アクセサで設定したほうがいい
	std::shared_ptr<CheckObstacleBall>		m_checkObstacleBall;// 見えない球を射出して視覚をシュミレート

private:
	const bool SettingWayPoint(const json11::Json& json_object);
	const bool CheckArrivalWayPoint();
	const bool CheckObstacle(std::shared_ptr<Tank> enemy);

private:
	//==================================================
	// 
	// 状態管理
	// 
	//==================================================

	//--------------------------------------------------
	// Name: class StateBase
	// Desc: 基底抽象ステート
	//--------------------------------------------------
	class StateBase
	{
	public:
		virtual void Initialize(AutomaticTank& owner) = 0;
		virtual void Update(AutomaticTank& owner) = 0;
	};

	//--------------------------------------------------
	// Name: class StateWait
	// Desc: 待機状態
	//--------------------------------------------------
	class StateWait : public StateBase
	{
	public:
		void Initialize(AutomaticTank& owner) override;
		void Update(AutomaticTank& owner)  override;
	};

	//--------------------------------------------------
	// Name: class StateMission
	// Desc: ミッション(移動を繰り返す)状態
	//--------------------------------------------------
	class StateMission : public StateBase
	{
	public:
		void Initialize(AutomaticTank& owner) override;
		void Update(AutomaticTank& owner)  override;
	};

	//--------------------------------------------------
	// Name: class StateCombat
	// Desc: 戦闘状態
	//--------------------------------------------------
	class StateCombat : public StateBase
	{
	public:
		StateCombat();
		void Initialize(AutomaticTank& owner) override;
		void Update(AutomaticTank& owner)  override;
	private:
		const bool CheckObstacle(AutomaticTank& owner);
	};

	//--------------------------------------------------
	// Name: class StateDead
	// Desc: 死亡状態
	//--------------------------------------------------
	class StateDead : public StateBase
	{
	public:
		void Initialize(AutomaticTank& owner) override;
		void Update(AutomaticTank& owner)  override;
	};
};