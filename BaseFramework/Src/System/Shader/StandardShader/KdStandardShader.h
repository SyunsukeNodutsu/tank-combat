//-----------------------------------------------------------------------------
// File: KdStandardShader.h
//
// Edit: 初期配布
//-----------------------------------------------------------------------------
#pragma once

//-----------------------------------------------------------------------------
// Name: class Scene
// Desc: 基本シェーダクラス
//-----------------------------------------------------------------------------
class KdStandardShader
{
	struct cbObject;
	struct cbMaterial;

public:
	//--------------------------------------------------
	// 設定・取得
	//--------------------------------------------------

	// ワールド行列セット
	void SetWorldMatrix(const Math::Matrix& m) { m_cb0.Work().mW = m; }

	// UVタイリング設定
	void SetUVTiling(const Math::Vector2& tiling) { m_cb0.Work().UVTiling = tiling; }

	// UVオフセット設定
	void SetUVOffset(const Math::Vector2& offset) { m_cb0.Work().UVOffset = offset; }

	// ライト有効/無効
	void SetLightEnable(bool enable) { m_cb0.Work().LightEnable = enable; }

	// フォグ有効/無効
	void SetFogEnable(bool enable) { m_cb0.Work().FogEnable = enable; }

	// アルファテストの閾値設定
	void SetAlphaTestThreshold(float alpha) { m_cb0.Work().AlphaTestThreshold = alpha; }

	// Set系でセットしたデータを、実際に定数バッファへ書き込む
	void WriteToCB()
	{
		m_cb0.Write();
		m_cb1_Material.Write();
	}

	// Object用定数バッファをそのまま返す
	KdConstantBuffer<cbObject>& ObjectCB() { return m_cb0; }

	// material用定数バッファをそのまま返す
	KdConstantBuffer<cbMaterial>& MaterialCB() { return m_cb1_Material; }

	//--------------------------------------------------
	// 描画
	//--------------------------------------------------

	// このシェーダをデバイスへセット
	void SetToDevice();

	// メッシュ描画
	void DrawMesh(const KdMesh* mesh, const std::vector<KdMaterial>& materials);

	//--------------------------------------------------
	// 初期化・解放
	//--------------------------------------------------

	// 初期化
	bool Init();

	// 解放
	void Release();

	// デストラクタ
	~KdStandardShader() { Release(); }

private:
	//--------------------------------------------------
	// ローカルデータ
	//--------------------------------------------------
	ID3D11VertexShader*	m_VS			= nullptr;	// 頂点シェーダー
	ID3D11InputLayout*	m_inputLayout	= nullptr;	// 頂点入力レイアウト
	ID3D11PixelShader*	m_PS			= nullptr;	// ピクセルシェーダー

	//--------------------------------------------------
	// 定数バッファ
	//--------------------------------------------------

	// 定数バッファ(オブジェクト単位更新)
	struct cbObject
	{
		Math::Matrix		mW;						 // ワールド行列　行列は16バイトx4バイトの64バイトなのでピッタリ。

		// UV操作
		Math::Vector2		UVOffset = { 0,0 };
		Math::Vector2		UVTiling = { 1,1 };

		int					LightEnable			= 1; // ライト有効
		int					FogEnable			= 1; // フォグ有効
		float				AlphaTestThreshold	= 0; // アルファテストの閾値
		float tmp[1];
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