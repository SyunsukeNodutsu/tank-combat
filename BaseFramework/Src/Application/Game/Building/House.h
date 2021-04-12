//-----------------------------------------------------------------------------
// File: House.h
//
// Edit: 2021/03/18 野筒隼輔
//-----------------------------------------------------------------------------
#pragma once
#include "Application/Game/GameObject.h"

//-----------------------------------------------------------------------------
// Name: class House
// Desc: 家クラス
//-----------------------------------------------------------------------------
class House : public GameObject
{
public:

	// コンストラクタ
	House();

	// 初期化
	void Deserialize(const json11::Json& json_object) override;

	// 更新
	void Update() override;

private:

};