//-----------------------------------------------------------------------------
// File: KdModelShader.h
//
// Edit: 2021/03/27 野筒隼輔
//-----------------------------------------------------------------------------
#pragma once

//-----------------------------------------------------------------------------
// Name: class KdModelShader
// Desc: モデル描画シェーダ
//-----------------------------------------------------------------------------
class KdModelShader
{
public:

	// コンストラクタ
	KdModelShader();
	// デストラクタ
	~KdModelShader() { Release(); }
	// 初期化
	bool Init();
	// 解放
	void Release();
	// このシェーダをデバイスへセット
	void SetToDevice();
	// メッシュ描画
	void DrawMesh(const KdMesh* mesh, const std::vector<KdMaterial>& materials);

	//--------------------------------------------------
	// 設定
	//--------------------------------------------------
	// ワールド行列セット
	void SetWorldMatrix(const Math::Matrix& m) { m_cb0.Work().mW = m; }
	// UVのオフセット設定
	void SetUvOffset(const Math::Vector2& offset) { m_cb0.Work().UVOffset = offset; }

private:
	ID3D11VertexShader*	m_VS;			// 頂点シェーダー
	ID3D11InputLayout*	m_inputLayout;	// 頂点入力レイアウト
	ID3D11PixelShader*	m_PS;			// ピクセルシェーダー


	// 定数バッファ(オブジェクト単位更新)
	struct cbObject
	{
		Math::Matrix	mW; // ワールド行列
		Math::Vector2	UVOffset = { 0.0f,0.0f };
		float			tmp[2];
	};
	KdConstantBuffer<cbObject> m_cb0;

	// 定数バッファ(マテリアル単位更新)
	struct cbMaterial
	{
		Math::Vector4	BaseColor;
		Math::Vector3	Emissive;
		float			Metallic;
		float			Roughness;
		float			tmp[3];
	};
	KdConstantBuffer<cbMaterial> m_cb1_Material;
};