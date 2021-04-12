//-----------------------------------------------------------------------------
// File: Tree.h
//
// Edit: 2021/03/18 野筒隼輔
//-----------------------------------------------------------------------------
#pragma once
#include "Application/Game/GameObject.h"

//-----------------------------------------------------------------------------
// Name: class Tree
// Desc: 木クラス 葉っぱの性質上.背面カリングをしないため空のDraw関数を所持
//-----------------------------------------------------------------------------
class Tree : public GameObject
{
public:

	// コンストラクタ
	Tree();
	// 初期化
	void Deserialize(const json11::Json& json_object);
	// 更新
	void Update() override;
	// Effect描画
	void DrawEffect() override;
	// シャドウマップ描画
	void DrawShadowMap() override;

	//--------------------------------------------------
	// 設定
	//--------------------------------------------------
	// 倒壊させる
	inline void Collapse() { m_isCollapse = true; }

private:
	std::shared_ptr<GameObject> m_spTreeGrass;			// 木の葉っぱ
	KdMatrix					m_treeGrassLocalMatrix;	// 木の葉っぱのローカル行列
	bool						m_isCollapse;			// 倒壊するかどうか
};