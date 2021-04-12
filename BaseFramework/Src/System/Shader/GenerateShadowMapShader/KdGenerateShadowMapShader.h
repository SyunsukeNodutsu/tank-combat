//-----------------------------------------------------------------------------
// File: KdGenerateShadowMapShader.h
//
// Edit: 2021/02/17 野筒隼輔
//-----------------------------------------------------------------------------
#pragma once

//-----------------------------------------------------------------------------
// Name: class KdGenerateShadowMapShader
// Desc: シャドウマップ(深度マップ)生成描画シェーダー
//-----------------------------------------------------------------------------
class KdGenerateShadowMapShader
{
public:

	// コンストラクタ
	KdGenerateShadowMapShader();

	// デストラクタ
	~KdGenerateShadowMapShader() { Release(); }

	// 初期化
	bool Init();

	// 解放
	void Release();

	// シャドウマップ生成描画開始
	void Begin();

	// シャドウマップ生成描画終了
	void End();

	// このシェーダをデバイスへセット
	void SetToDevice();

	// メッシュ描画
	void DrawMeshDepth(const KdMesh* mesh, const std::vector<KdMaterial>& materials);

	//--------------------------------------------------
	// 設定.所得
	//--------------------------------------------------
	// 平行光用深度マップ取得
	std::shared_ptr<KdTexture> GetDirShadowMap() const { return m_dirLightShadowMap; }
	// ワールド行列セット
	void SetWorldMatrix(const Math::Matrix& m) { m_cb0.Work().mW = m; }

private:
	ID3D11VertexShader*				m_VS;					// 頂点シェーダー
	ID3D11InputLayout*				m_inputLayout;			// 頂点入力レイアウト
	ID3D11PixelShader*				m_PS;					// ピクセルシェーダー
	std::shared_ptr<KdTexture>		m_dirLightShadowMap;	// 平行光用 深度マップ
	std::shared_ptr<KdTexture>		m_dirLightZBuffer;		// 平行光用 Zバッファ
	// 記憶用
	ID3D11RenderTargetView*			m_saveRT;				// 記憶用.RenderTarget
	ID3D11DepthStencilView*			m_saveZ;				// 記憶用.Zバッファ
	UINT							m_numVP;
	D3D11_VIEWPORT					m_saveVP;				// 記憶用.ビューポート
	// オプション
	KdVector3						m_lightCameraLook;		// ライトカメラの向き

	// 定数バッファ
	struct cbObject
	{
		KdMatrix mW;			// ワールド行列
		KdMatrix mLightVP;		// ライトカメラのビュー行列 * 射影行列
	};
	KdConstantBuffer<cbObject> m_cb0;

private:
	void SettingLightCamera();
};
