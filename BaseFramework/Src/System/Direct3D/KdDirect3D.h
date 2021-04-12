//-----------------------------------------------------------------------------
// File: KdDirect3D.h
//
// Edit: 2021/03/29 野筒隼輔 (初期配布)
//-----------------------------------------------------------------------------
#pragma once

//--------------------------------------------------
// 前方宣言
//--------------------------------------------------
class KdTexture;
enum class KdSamplerFilterMode;
enum class KdSamplerAddressingMode;
enum class KdBlendMode;

//-----------------------------------------------------------------------------
// Name: class KdDirect3D
// Desc: Direct3Dクラス, Direct3D11に必要な変数やよく行う操作をまとめています
//-----------------------------------------------------------------------------
class KdDirect3D
{
public:

	// デストラクタ
	~KdDirect3D() { Release(); }
	// 初期化
	bool Init(HWND hWnd, int w, int h, bool deviceDebug, std::string& errMsg);
	// 解放
	void Release();
	// 頂点を描画する簡易的な関数
	// ・topology		… 頂点をどのような形状で描画するか　D3D_PRIMITIVE_TOPOLOGYマクロを使用
	// ・vertexCount	… 頂点数
	// ・pVertexStream	… 頂点配列の先頭アドレス
	// ・stride			… １頂点のバイトサイズ
	void DrawVertices(D3D_PRIMITIVE_TOPOLOGY topology, int vertexCount, const void* pVertexStream, UINT stride);

	//--------------------------------------------------
	// 取得
	//--------------------------------------------------
	// インスタンスの所得
	static KdDirect3D& GetInstance() { static KdDirect3D instance; return instance; }
	// Direct3Dデバイス取得
	ID3D11Device* GetDev() { return m_pDevice; }
	// Direct3Dデバイスコンテキスト取得
	ID3D11DeviceContext* GetDevContext() { return m_pDeviceContext; }
	// SwapChain取得
	IDXGISwapChain* GetSwapChain() { return m_pGISwapChain; }
	// バックバッファ取得
	std::shared_ptr<KdTexture> GetBackBuffer() { return m_backBuffer; }
	// Zバッファ取得
	std::shared_ptr<KdTexture> GetZBuffer() { return m_zBuffer; }
	// ビューポート取得
	void GetViewport(Math::Viewport& out) const;
	// 1x1 白テクスチャ取得
	std::shared_ptr<KdTexture> GetWhiteTex() const { return m_texWhite; }
	// 1x1 Z向き法線マップ取得
	std::shared_ptr<KdTexture> GetNormalTex() const { return m_texNormal; }

	//--------------------------------------------------
	// パイプラインステート作成関数
	//--------------------------------------------------

	// 深度ステンシルステート作成
	// ・device				… D3Dデバイス
	// ・zEnable			… 深度テスト有効
	// ・zWriteEnable		… 深度書き込み有効
	ID3D11DepthStencilState* CreateDepthStencilState(bool zEnable, bool zWriteEnable) const;

	// ラスタライザーステート作成
	// ・device				… D3Dデバイス
	// ・cullMode			… カリングモード
	// ・fillMode			… 三角形の描画モード
	// ・depthClipEnable	… Zクリッピングを有効にする
	// ・scissorEnable		… 切り抜き範囲を有効にする
	ID3D11RasterizerState* CreateRasterizerState(D3D11_CULL_MODE cullMode, D3D11_FILL_MODE fillMode, bool depthClipEnable, bool scissorEnable) const;

	// サンプラーステート作成
	// ・device				… D3Dデバイス
	// ・filterType			… 0:補間なし 1:線形フィルタ 2:異方性フィルタ
	// ・maxAnisotropy		… 異方性フィルタ時の最大値　2, 4, 6, 8, 10, 12, 14, 16 のいずれか
	// ・addressingMode		… テクスチャアドレッシングモード 0:Wrap 1:Clamp
	// ・comparisonModel	… 比較モードON　シャドウマッピングなどで使用する
	ID3D11SamplerState* CreateSamplerState(KdSamplerFilterMode filterType, UINT maxAnisotropy, KdSamplerAddressingMode addressingMode, bool comparisonModel) const;

	// ブレンドステート作成
	// ・device				… D3Dデバイス
	// ・mode				… 合成モード
	ID3D11BlendState* CreateBlendState(KdBlendMode mode) const;

private:
	ID3D11Device*				m_pDevice;						// Direct3D11の中心になるクラス。全体の管理とバッファ、シェーダ、テクスチャなどのリソース作成などを行う。D3D9とは違って、このクラスは描画関係のメンバ関数を持たない。
	ID3D11DeviceContext*		m_pDeviceContext;				// 描画処理を行うクラス。
																// 内部的には、レンダリングコマンドと呼ばれるバイナリデータを作成し、GPUに送る。GPUがレンダリングコマンドを解析することで実際の描画処理が行われる。
	IDXGISwapChain*				m_pGISwapChain;					// ウインドウへの表示やダブルバッファリングなどを行うクラス。マルチサンプリング、リフレッシュレートなどの設定もできるみたい。

	// バッファ
	std::shared_ptr<KdTexture>	m_backBuffer;					// バックバッファ
	std::shared_ptr<KdTexture>	m_zBuffer;						// Zバッファ

	// DrawVertices用 頂点バッファ
	KdBuffer					m_tempFixedVertexBuffer[10];	// 固定長 頂点バッファ
	KdBuffer					m_tempVertexBuffer;				// 可変長 頂点バッファ

	//------------------------
	// 便利テクスチャ
	//------------------------
	std::shared_ptr<KdTexture>	m_texWhite;						// 1x1 白テクスチャ
	std::shared_ptr<KdTexture>	m_texNormal;					// 1x1 Z向き法線マップ

private:
	KdDirect3D();
};

// サンプラステート
enum class KdSamplerFilterMode
{
	Point,			// ポイントフィルタ
	Linear,			// 線形フィルタ
	Anisotropic,	// 異方性フィルタ
};

// 
enum class KdSamplerAddressingMode
{
	Wrap,			// ラップモード
	Clamp,			// クランプモード
};

// ブレンドモード用定数
enum class KdBlendMode
{
	NoBlend,		// ブレンドをしない
	Alpha,			// 半透明ブレンド
	Add				// 加算ブレンド
};

//-----------------------------------------------------------------------------
// Define: 簡単にアクセス
//-----------------------------------------------------------------------------
#define D3D KdDirect3D::GetInstance()