//-----------------------------------------------------------------------------
// File: KdModel.h
//
// Edit: 2021/03/07 野筒隼輔
//-----------------------------------------------------------------------------
#pragma once

//--------------------------------------------------
// 前方宣言
//--------------------------------------------------
struct KdAnimationData;

//-----------------------------------------------------------------------------
// Name: class KdModel
// Desc: モデル管理クラス
//-----------------------------------------------------------------------------
class KdModel
{
public:

	// ノード構造体
	struct Node
	{
		std::string				m_name;				// ノード名
		KdMatrix				m_localTransform;	// 変換行列
		std::shared_ptr<KdMesh> m_spMesh;			// メッシュ情報
	};

public:

	// コンストラクタ
	KdModel();

	// デストラクタ
	~KdModel() { Release(); }

	// モデルデータ読み込み
	const bool Load(const std::string& filepath);

	//--------------------------------------------------
	// 所得
	//--------------------------------------------------
	// メッシュ読み込み
	const std::shared_ptr<KdMesh> GetMesh(UINT index) const { return index < m_originalNodes.size() ? m_originalNodes[index].m_spMesh : nullptr; }
	// マテリアル読み込み
	const std::vector<KdMaterial>& GetMaterials() const { return m_materials; }
	// ノード配列所得
	const std::vector<Node>& GetOriginalNodes() const { return m_originalNodes; }
	// アニメーションデータ所得
	const std::shared_ptr<KdAnimationData> GetAnimation(const std::string& aAnimName) const;
	// 文字列を元にノードの検索.所得
	inline Node* FindNode(const std::string name)
	{
		for (auto&& node : m_originalNodes) {
			if (node.m_name == name) return &node;
		}
		return nullptr;
	}

private:
	std::vector<Node>								m_originalNodes;	// データのノード配列
	std::vector<KdMaterial>							m_materials;		// マテリアル配列
	std::vector<std::shared_ptr<KdAnimationData>>	m_spAnimations;		// アニメーションデータリスト

private:
	void Release();
};