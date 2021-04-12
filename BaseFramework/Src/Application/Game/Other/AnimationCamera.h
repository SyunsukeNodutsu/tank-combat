//-----------------------------------------------------------------------------
// File: AnimationCamera.h
//
// Edit: 2021/03/07 野筒隼輔
//-----------------------------------------------------------------------------
#pragma once
#include "Application/Game/GameObject.h"

//-----------------------------------------------------------------------------
// Name: class AnimationCamera
// Desc: アニメーションカメラクラス
//-----------------------------------------------------------------------------
class AnimationCamera : public GameObject
{
public:

	// コンストラクタ
	AnimationCamera();
	// 更新 2点間の補完を行う
	void Update() override;
	// 情報の設定
	void SetInfo(const std::shared_ptr<GameObject>& start, const std::shared_ptr<GameObject>& end, float speed = 0.01f);

private:
	std::weak_ptr<GameObject>	m_wpStart;	// 開始地点のオブジェクト
	std::weak_ptr<GameObject>	m_wpEnd;	// 終了地点のオブジェクト
	float						m_progress;	// 進行具合(0～1)
	float						m_speed;	// 進行スピード
};