//-----------------------------------------------------------------------------
// File: TitleTank.h
//
// Edit: 2021/03/19 野筒隼輔
//-----------------------------------------------------------------------------
#pragma once
#include "Application/Game/Weapon/Tank.h"

//-----------------------------------------------------------------------------
// Name: class TitleTank
// Desc: タイトルの戦車クラス 前進するだけのバカ
//-----------------------------------------------------------------------------
class TitleTank : public Tank
{
public:

	// 初期化
	void Deserialize(const json11::Json& json_object) override;
	// 状態ごとの更新 ※TankクラスのUpdate内で呼ばれる
	void UpdateByState() override;
};