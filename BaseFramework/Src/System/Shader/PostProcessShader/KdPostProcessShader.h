//-----------------------------------------------------------------------------
// File: PostProcessShader.h
//
// Edit: 授業
//-----------------------------------------------------------------------------
#pragma once

//--------------------------------------------------
// 前方宣言
//--------------------------------------------------
struct KdBlurTexture;

//-----------------------------------------------------------------------------
// Name: class KdPostProcessShader
// Desc: ポストエフェクトシェーダ
//-----------------------------------------------------------------------------
class KdPostProcessShader
{
public:
	// コンストラクタ
	KdPostProcessShader();
	
	// デストラクタ
	~KdPostProcessShader() { Release(); }

	// 初期化
	bool Init();

	// 解放
	void Release();

	// 描画(指定色合成)
	void ColorDraw(const KdTexture* a_pTex, const Math::Vector4& a_color = { 1,1,1,1 });

	// 描画(ぼかし)
	void BlurDraw(const KdTexture* a_pTex, const Math::Vector2& a_dir);

	// 高輝度抽出ぼかし描画
	void GenerateBlur(KdBlurTexture& a_pBlurTex, const KdTexture* a_pSrcTex);

	// フィルタリング
	void BrightFiltering(const KdTexture* a_pDestRT, const KdTexture* a_pSrcTex);

public:
	// 頂点情報
	struct Vertex
	{
		Math::Vector3 Pos;
		Math::Vector2 UV;
	};

private:
	// 定数バッファ(指定色)
	struct cbColor
	{
		Math::Vector4 Color = { 1,1,1,1 };
	};

	// 定数バッファ(ぼかし)
	struct cbBlur
	{
		Math::Vector4 Offset[31];
	};

private:
	ID3D11VertexShader*			m_VS;			// 頂点シェーダ
	ID3D11InputLayout*			m_inputLayout;	// 頂点インプットレイアウト
	ID3D11PixelShader*			m_colorPS;		// ピクセルシェーダ(色合成)
	ID3D11PixelShader*			m_blurPS;		// ピクセルシェーダ(ぼかし)
	ID3D11PixelShader*			m_HBrightPS;	// ピクセルシェーダ(ぼかし)
	KdConstantBuffer<cbColor>	m_cb0_Color;	// 定数バッファ(指定色)
	KdConstantBuffer<cbBlur>	m_cb0_Blur;		// 定数バッファ(ぼかし)
};

//-----------------------------------------------------------------------------
// Name: struct KdBlurTexture
// Desc: 高輝度抽出用のTexdture
//-----------------------------------------------------------------------------
struct KdBlurTexture
{
	// 生成
	void Create(int a_width, int a_height)
	{
		int divideValue = 2;
		for (int i = 0; i < 5; i++)
		{
			m_rt[i][0] = std::make_shared<KdTexture>();
			m_rt[i][0]->CreateRenderTarget(a_width / divideValue, a_height / divideValue, DXGI_FORMAT_R16G16B16A16_FLOAT);

			m_rt[i][1] = std::make_shared<KdTexture>();
			m_rt[i][1]->CreateRenderTarget(a_width / divideValue, a_height / divideValue, DXGI_FORMAT_R16G16B16A16_FLOAT);
			divideValue *= 2;
		}
	}

	// テクスチャ
	std::shared_ptr<KdTexture>	m_rt[5][2];
};