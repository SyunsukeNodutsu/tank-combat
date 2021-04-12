//-----------------------------------------------------------------------------
// File: FixedCamera.h
//
// Edit: 2021/03/29 野筒隼輔
//-----------------------------------------------------------------------------
#pragma once
#include "Application/Game/GameObject.h"

//-----------------------------------------------------------------------------
// Name: class FixedCamera
// Desc: 設置カメラクラス
//-----------------------------------------------------------------------------
class FixedCamera : public GameObject
{
public:

	// コンストラクタ
	FixedCamera();
	// 初期化
	void Deserialize(const json11::Json& json_object) override;
	// 更新
	void Update() override;
	// 移動させる
	void Move(const KdVector3 aVec) { m_viewPosition += aVec; }

private:
	KdVector3	m_viewPosition;	// 注視点
	POINT		m_prevMousePos;	// マウス.1フレーム前の座標
};