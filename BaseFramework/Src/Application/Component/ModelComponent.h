//-----------------------------------------------------------------------------
// File: ModelComponent.h
//
// Edit: 2021/03/10 野筒隼輔
//-----------------------------------------------------------------------------
#pragma once
#include "Application/Game/GameObject.h"

//-----------------------------------------------------------------------------
// Name: class ModelComponent
// Desc: モデル管理クラス
//-----------------------------------------------------------------------------
class ModelComponent
{
public:

	// コンストラクタ
	ModelComponent(GameObject& owner);
	// 描画
	void Draw();
	// シャドウマップ生成描画
	void DrawShadowMap();
	// エフェクト描画
	void DrawEffect();

	//--------------------------------------------------
	// 所得
	//--------------------------------------------------
	// 描画有効フラグの所得
	const bool IsEnable() const { return m_isEnable; }
	// モデル所得
	std::shared_ptr<KdModel> GetModel() { return m_spModel; }
	// ノード(パーツ)の所得
	std::vector<KdModel::Node>& GetChangeableNodes() { return m_coppiedNodes; }
	// メッシュ所得
	inline const std::shared_ptr<KdMesh> GetMesh(UINT index) const
	{
		if (index > m_coppiedNodes.size()) { return nullptr; }
		return m_coppiedNodes[index].m_spMesh;
	}
	// アニメーションデータ所得
	const std::shared_ptr<KdAnimationData> GetAnimation(const std::string& name) const
	{
		if (m_spModel == nullptr) { return nullptr; }
		return m_spModel->GetAnimation(name);
	}
	// 各ノードを名前検索
	inline KdModel::Node* FindNode(const std::string& name)
	{
		for (auto&& node : m_coppiedNodes) {
			if (node.m_name == name) { return &node; }
		}
		return nullptr;
	}

	//--------------------------------------------------
	// 設定
	//--------------------------------------------------
	// モデル設定
	void SetModel(const std::shared_ptr<KdModel>& model);
	// 描画有効フラグの設定
	void SetEnable(const bool enable) { m_isEnable = enable; }
	// UVスクロールを行うか設定
	void SetUVScroll(const bool enable) { m_isUvScroll = enable; }
	// UVのMove値を設定
	void SetUVMoveNum(const Math::Vector2 vec) { m_uvMoveNum = vec; }

protected:
	GameObject&					m_owner;		// オーナー
	std::shared_ptr<KdModel>	m_spModel;		// モデル
	std::vector<KdModel::Node>	m_coppiedNodes;	// ノードコンテナ ※個別管理のためコピーして保存
	Math::Vector2				m_uvOffset;		// UVのOffset(初期)値
	Math::Vector2				m_uvMoveNum;	// UVのスクロール値
	bool						m_isEnable;		// 有効かどうか
	bool						m_isUvScroll;	// UVスクロールを行うか

private:
	void DrawOBB(const DirectX::BoundingOrientedBox& obb);
};