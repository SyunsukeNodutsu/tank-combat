//-----------------------------------------------------------------------------
// File: Aircraft.h
//
// Edit: 2021/02/19 野筒隼輔
//-----------------------------------------------------------------------------
#pragma once
#include "Application/Game/GameObject.h"

//-----------------------------------------------------------------------------
// Name: class Aircraft
// Desc: 飛行機クラス
//-----------------------------------------------------------------------------
class Aircraft : public GameObject
{
public:

	// コンストラクタ
	Aircraft();
	// 初期化
	void Deserialize(const json11::Json& json_object) override;
	// 更新
	void Update() override;
	// エフェクト描画
	void DrawEffect() override;

private:
	std::shared_ptr<GameObject>	m_spPropeller;	// 先頭のプロペラ
	KdMatrix					m_localPropMat;	// プロペラのローカル行列
	KdTrailPolygon				m_trailSmoke;	// 飛行機雲ポリゴン
	float						m_trailRotate;	// 飛行機雲の回転
};