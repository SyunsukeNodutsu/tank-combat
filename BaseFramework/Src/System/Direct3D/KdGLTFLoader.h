//-----------------------------------------------------------------------------
// File: KdGLTFLoader.h
//
// Edit: 2021/03/29 野筒隼輔 (初期配布)
//-----------------------------------------------------------------------------
#pragma once

//-----------------------------------------------------------------------------
// Name: struct KdGLTFMaterial
// Desc: マテリアル
//-----------------------------------------------------------------------------
struct KdGLTFMaterial
{
public:
	std::string		Name;						// マテリアル名
	std::string		AlphaMode = "OPAQUE";		// "OPAQUE" : レンダリングされた出力は完全に不透明で、アルファ値は無視されます。
												// "MASK"   : レンダリングされた出力は、アルファ値と指定されたアルファカットオフ値に応じて、完全に不透明または完全に透明になります。このモードは、木の葉やワイヤーフェンスなどのジオメトリをシミュレートするために使用されます。
												// "BLEND"  : 

	float			AlphaCutoff = 0.5f;			// MASKモード時に、カットオフの閾値として使用　それ以外のモードでは使用されない
	bool			DoubleSided = false;		// 両面か？

	//------------------------------
	// PBR材質データ
	//------------------------------
	std::string		BaseColorTexture;			// 基本色テクスチャのファイル名
	Math::Vector4	BaseColor = { 1,1,1,1 };	// 上記テクスチャのRGBAのスケーリング要素

	// 金属性、粗さ
	std::string		MetallicRoughnessTexture;	// メタリックとラフネスのテクスチャ　青成分 = メタリック 緑成分 = ラフネス
	float			Metallic = 1.0f;			// 上記テクスチャのメタリック要素の乗算用　テクスチャが無い時は乗算ではなくそのまま使用する
	float			Roughness = 1.0f;			// 上記テクスチャのラフネス要素の乗算用　テクスチャが無い時は乗算ではなくそのまま使用する

	// エミッシブ：自己発光 つまり表面から放出される光　RGBのみ使用
	std::string		EmissiveTexture;			// エミッシブテクスチャ　RGBを使用
	Math::Vector3	Emissive = { 0,0,0 };		// 上記テクスチャのRGBのスケーリング要素

	//------------------------------
	// その他テクスチャ
	//------------------------------
	std::string		NormalTexture;				// 法線マップテクスチャ
	std::string		OcclusionTexture;			// 光の遮蔽度テクスチャ　赤成分のみ使用
};

//-----------------------------------------------------------------------------
// Name: struct KdGLTFNode
// Desc: ノード １つのメッシュやマテリアルなど
//-----------------------------------------------------------------------------
struct KdGLTFNode
{
public:
	//---------------------------
	// 基本情報
	//---------------------------
	std::string			Name;			// 名前
	std::vector<int>	Children;		// 子Indexリスト
	int					Parent = -1;	// 親Index
	Math::Matrix		LocalTransform;	// ローカル行列
	Math::Matrix		WorldTransform;	// ワールド行列

	//---------------------------
	// Mesh専用情報
	//---------------------------
	bool				IsMesh = false;	//
	struct Mesh
	{
	public:
		std::vector<KdMeshVertex>	Vertices;	// 頂点配列
		std::vector<KdMeshFace>		Faces;		// 面情報配列
		std::vector<KdMeshSubset>	Subsets;	// サブセット情報配列
	};
	Mesh				Mesh;			// メッシュ

};

//-----------------------------------------------------------------------------
// Name: struct KdGLTFAnimationData
// Desc: アニメーション
//-----------------------------------------------------------------------------
struct KdGLTFAnimationData
{
public:
	std::string		m_name;				// アニメーション名
	float			m_maxLength = 0.0f;	// アニメーションの長さ

	// 1ノードのアニメーションデータ
	struct Node
	{
	public:
		int									m_nodeOffset = -1;	// 対象ノードのOffset

		// 各チャンネル
		std::vector<KdAnimKeyQuaternion>	m_rotations;		// 回転キーリスト
		std::vector<KdAnimKeyVector3>		m_scales;			// 拡大キーリスト
		std::vector<KdAnimKeyVector3>		m_translations;		// 位置キーリスト
	};

	// 全ノード用アニメーションデータ
	std::vector<std::shared_ptr<Node>> m_nodes;
};

//-----------------------------------------------------------------------------
// Name: struct KdGLTFModel
// Desc: モデルデータ
//-----------------------------------------------------------------------------
struct KdGLTFModel
{
public:
	std::vector<KdGLTFNode>								Nodes;		// 全ノードデータ
	std::vector<KdGLTFNode*>							RootNodes;	// 全ノード中のルートノードのみの参照リスト
	std::vector<KdGLTFMaterial>							Materials;	// マテリアル一覧
	std::vector<std::shared_ptr<KdGLTFAnimationData>>	Animations;	// アニメーションデータリスト
};

//-----------------------------------------------------------------------------
// Name: KdLoadGLTFModel()
// Desc: GLTF形式の3Dモデルを読み込む ※Tinygltf...github:https://github.com/syoyo/tinygltf
//-----------------------------------------------------------------------------
std::shared_ptr<KdGLTFModel> KdLoadGLTFModel(const std::string& path);