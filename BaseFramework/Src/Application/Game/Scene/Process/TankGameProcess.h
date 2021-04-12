//-----------------------------------------------------------------------------
// File: TankGameProcess.h
//
// Edit: 2021/03/19 野筒隼輔
//-----------------------------------------------------------------------------
#pragma once
#include "Application/Game/GameObject.h"

//-----------------------------------------------------------------------------
// Name: class TankGameProcess
// Desc: 戦車ゲーム(ほんへ)の管理
//-----------------------------------------------------------------------------
class TankGameProcess : public GameObject
{
public:

	// コンストラクタ
	TankGameProcess();
	// 初期化
	void Deserialize(const json11::Json& json_object) override;
	// 更新
	void Update() override;

private:
	bool m_onceEmitRadio; // 初回の無線

private:
	void OnceEmitRadio();
};