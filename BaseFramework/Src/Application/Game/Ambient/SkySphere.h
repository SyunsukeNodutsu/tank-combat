//-----------------------------------------------------------------------------
// File: SkySphere.h
//
// Edit: 2021/03/17 野筒隼輔
//-----------------------------------------------------------------------------
#pragma once
#include "Application/Game/GameObject.h"

//-----------------------------------------------------------------------------
// Name: class SkySphere
// Desc: 空の球体クラス
//-----------------------------------------------------------------------------
class SkySphere : public GameObject
{
public:

	// コンストラクタ
	SkySphere();

	// 初期化
	void Deserialize(const json11::Json& json_object) override;

	// 更新
	void Update() override;

	// 描画
	void Draw() override;

private:
	float m_rotateSpeed;
};