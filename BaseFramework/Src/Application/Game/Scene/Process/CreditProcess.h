//-----------------------------------------------------------------------------
// File: CreditProcess.h
//
// Edit: 2021/03/22 野筒隼輔
//-----------------------------------------------------------------------------
#pragma once
#include "Application/Game/GameObject.h"

//-----------------------------------------------------------------------------
// Name: class CreditProcess
// Desc: クレジットの管理
//-----------------------------------------------------------------------------
class CreditProcess : public GameObject
{
public:

	// コンストラクタ
	CreditProcess();
	// 初期化
	void Deserialize(const json11::Json& json_object) override;
	// 更新
	void Update() override;
	// 2D描画
	void DrawSprite() override;

private:
	std::shared_ptr<KdTexture> m_spBackTexture;
	std::shared_ptr<KdTexture> m_spKeyTipsTexture;
};