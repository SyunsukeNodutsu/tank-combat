//-----------------------------------------------------------------------------
// File: KdModel.cpp
//
// Edit: 2021/03/07 野筒隼輔
//-----------------------------------------------------------------------------
#include "KdModel.h"
#include "KdGLTFLoader.h"

//-----------------------------------------------------------------------------
// Name: KdModel()
// Desc: コンストラクタ
//-----------------------------------------------------------------------------
KdModel::KdModel()
	: m_originalNodes()
	, m_materials()
	, m_spAnimations()
{
}

//-----------------------------------------------------------------------------
// Name: Load()
// Desc: モデル読み込み
//-----------------------------------------------------------------------------
const bool KdModel::Load(const std::string& filepath)
{
	// ファイルの完全パスを所得
	std::string fileDir = KdGetDirFromPath(filepath);

	// GLTF読み込み
	std::shared_ptr<KdGLTFModel> spGltfModel = KdLoadGLTFModel(filepath);
	if (spGltfModel == nullptr) { return false; }

	// ノード格納場所のメモリ確保
	m_originalNodes.resize(spGltfModel->Nodes.size());

	//--------------------------------------------------
	// メッシュデータ
	//--------------------------------------------------
	for (UINT i = 0; i < spGltfModel->Nodes.size(); ++i)
	{
		// 入力元ノード
		const KdGLTFNode& rSrcNode = spGltfModel->Nodes[i];

		// 出力先ノード
		Node& rDstNode = m_originalNodes[i];

		// ノード情報の設定
		rDstNode.m_name = rSrcNode.Name;
		rDstNode.m_localTransform = rSrcNode.LocalTransform;

		// ノードの内容がメッシュだったら
		if (rSrcNode.IsMesh)
		{
			// リストにメッシュのインスタンス化
			rDstNode.m_spMesh = std::make_shared<KdMesh>();

			// メッシュ情報の作成
			if (rDstNode.m_spMesh)
			{
				rDstNode.m_spMesh->Create(
					rSrcNode.Mesh.Vertices,
					rSrcNode.Mesh.Faces,
					rSrcNode.Mesh.Subsets
				);
			}
		}
	}

	//--------------------------------------------------
	// マテリアルデータ
	//--------------------------------------------------
	m_materials.resize(spGltfModel->Materials.size());
	for (UINT i = 0; i < m_materials.size(); ++i)
	{
		const KdGLTFMaterial& rSrcMaterial = spGltfModel->Materials[i];
		KdMaterial& rDstMaterial = m_materials[i];

		// 情報設定
		rDstMaterial.Name = rSrcMaterial.Name;
		rDstMaterial.BaseColor = rSrcMaterial.BaseColor;
		rDstMaterial.BaseColorTex = std::make_shared<KdTexture>();
		rDstMaterial.BaseColorTex = KdResFac.GetTexture(fileDir + rSrcMaterial.BaseColorTexture);
		if (rDstMaterial.BaseColorTex == nullptr)
		{
			rDstMaterial.BaseColorTex = D3D.GetWhiteTex();
		}

		// 金属性・粗さ
		rDstMaterial.Metallic = rSrcMaterial.Metallic;
		rDstMaterial.Roughness = rSrcMaterial.Roughness;
		rDstMaterial.MetallicRoughnessTex = std::make_shared<KdTexture>();
		if (rDstMaterial.MetallicRoughnessTex->Load(fileDir + rSrcMaterial.MetallicRoughnessTexture) == false)
		{
			// 読み込めなかった場合は、代わりに白画像を使用
			rDstMaterial.MetallicRoughnessTex = D3D.GetWhiteTex();
		}

		// 法線マップ
		rDstMaterial.NormalTex = std::make_shared<KdTexture>();
		if (rDstMaterial.NormalTex->Load(fileDir + rSrcMaterial.NormalTexture) == false)
		{
			// 読み込めなかった場合は、代わりにZ向き法線マップを使用
			rDstMaterial.NormalTex = D3D.GetNormalTex();
		}

		//自己発光
		rDstMaterial.Emissive = rSrcMaterial.Emissive;
		rDstMaterial.EmissiveTex = std::make_shared<KdTexture>();
		if (rDstMaterial.EmissiveTex->Load(fileDir + rSrcMaterial.EmissiveTexture) == false)
		{
			// 読み込めなかった場合は、代わりに白画像を使用
			rDstMaterial.MetallicRoughnessTex = D3D.GetWhiteTex();
		}
	}

	//--------------------------------------------------
	// アニメーションデータ
	//--------------------------------------------------
	m_spAnimations.resize(spGltfModel->Animations.size());
	for (UINT i = 0; i < m_spAnimations.size(); ++i)
	{
		const KdGLTFAnimationData& rSrcAnimation = *spGltfModel->Animations[i];

		m_spAnimations[i] = std::make_shared<KdAnimationData>();
		if (m_spAnimations[i] == nullptr)
		{
			assert(0 && "error");
		}

		KdAnimationData& rDstAnimation = *(m_spAnimations[i]);
		rDstAnimation.m_name = rSrcAnimation.m_name;
		rDstAnimation.m_maxLength = rSrcAnimation.m_maxLength;
		rDstAnimation.m_nodes.resize(rSrcAnimation.m_nodes.size());

		for (UINT i = 0; i < rDstAnimation.m_nodes.size(); ++i)
		{
			rDstAnimation.m_nodes[i].m_nodeOffset = rSrcAnimation.m_nodes[i]->m_nodeOffset;
			rDstAnimation.m_nodes[i].m_translations = rSrcAnimation.m_nodes[i]->m_translations;
			rDstAnimation.m_nodes[i].m_rotations = rSrcAnimation.m_nodes[i]->m_rotations;
		}
	}

	return true;
}

//-----------------------------------------------------------------------------
// Name: GetAnimation()
// Desc: アニメーションデータ所得
//-----------------------------------------------------------------------------
const std::shared_ptr<KdAnimationData> KdModel::GetAnimation(const std::string& name) const
{
	for (auto&& anim : m_spAnimations) {
		if (anim->m_name == name) { return anim; }
	}

	return nullptr;
}

//-----------------------------------------------------------------------------
// Name: Release()
// Desc: 解放
//-----------------------------------------------------------------------------
void KdModel::Release()
{
	m_materials.clear();
	m_originalNodes.clear();
	m_spAnimations.clear();
}