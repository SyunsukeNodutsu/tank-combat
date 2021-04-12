//-----------------------------------------------------------------------------
// File: Human.h
// 
// Edit: 2021/03/23 野筒隼輔
//-----------------------------------------------------------------------------
#pragma once
#include "Application/Game/GameObject.h"

//-----------------------------------------------------------------------------
// Name: class Human
// Desc: 人型クラス アニメーションのテスト用
//-----------------------------------------------------------------------------
class Human : public GameObject
{
public:

	// コンストラクタ
	Human();
	// 初期化
	void Deserialize(const json11::Json& json_object) override;
	// 更新
	void Update() override;
	// アニメーション設定
	void SetAnimation(const char* name);

private:
	KdBrendAnimator m_animator;			// アニメーション管理
	float			m_animationSpeed;	// アニメーション速度
};