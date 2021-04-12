//-----------------------------------------------------------------------------
// File: Editorcamera.h
//
// Edit: 2021/03/08 野筒隼輔
//-----------------------------------------------------------------------------
#pragma once
#include "Application/Game/GameObject.h"

//-----------------------------------------------------------------------------
// Name: class EditorCamera
// Desc: 編集カメラクラス
//-----------------------------------------------------------------------------
class EditorCamera : public GameObject
{
public:

	// コンストラクタ
	EditorCamera();
	// 初期化
	void Deserialize(const json11::Json& json_object) override;
	// 更新
	void Update() override;

private:
	KdVector3	m_viewPosition;	// 注視点
	POINT		m_prevMousePos;	// 前フレームのマウス座標
};