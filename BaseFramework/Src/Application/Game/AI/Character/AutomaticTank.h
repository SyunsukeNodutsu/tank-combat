//-----------------------------------------------------------------------------
// File: AutomaticTank.h
//
// Edit: 2021/03/19 野筒隼輔
//-----------------------------------------------------------------------------
#pragma once
#include "Application/Game/Weapon/Tank.h"

//-----------------------------------------------------------------------------
// Name: class AutomaticTank
// Desc: 自動戦車クラス
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

private:
	std::shared_ptr<StateBase> m_spState; // 状態

private:
	//--------------------------------------------------
	// 状態管理 ステートマシン
	//--------------------------------------------------

	// 基底抽象ステート
	class StateBase
	{
	public:
		virtual void Initialize(AutomaticTank& owner) = 0;
		virtual void Update(AutomaticTank& owner) = 0;
	};

	// ミッション(移動を繰り返す)状態
	class StateMission : public StateBase
	{
	public:
		void Initialize(AutomaticTank& owner) override;
		void Update(AutomaticTank& owner)  override;
	};

	// 死亡状態
	class StateDead : public StateBase
	{
	public:
		void Initialize(AutomaticTank& owner) override;
		void Update(AutomaticTank& owner)  override;
	};
};