//-----------------------------------------------------------------------------
// File: PostProcessShader.cpp
//
// Edit: 授業
//-----------------------------------------------------------------------------
#include "KdPostProcessShader.h"

KdPostProcessShader::KdPostProcessShader()
	: m_VS(nullptr)
	, m_inputLayout(nullptr)
	, m_colorPS(nullptr)
	, m_blurPS(nullptr)
	, m_HBrightPS(nullptr)
	, m_cb0_Color()
	, m_cb0_Blur()
{
}

bool KdPostProcessShader::Init()
{
	//--------------------------------------------------
	// 頂点シェーダ
	//--------------------------------------------------
	{
		#include "KdPostProcess_VS.inc"

		// 生成
		if (FAILED(D3D.GetDev()->CreateVertexShader(compiledBuffer, sizeof(compiledBuffer), nullptr, &m_VS)))
		{
			assert(0 && "頂点シェーダー作成失敗");
			Release();
			return false;
		}

		// 1頂点ごとの情報
		std::vector<D3D11_INPUT_ELEMENT_DESC> layout = {
			{ "POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT,		0,  0, D3D11_INPUT_PER_VERTEX_DATA, 0 },
			{ "TEXCOORD", 0, DXGI_FORMAT_R32G32_FLOAT,			0, 12, D3D11_INPUT_PER_VERTEX_DATA, 0 },
		};

		// 頂点インプットレイアウト生成
		if (FAILED(D3D.GetDev()->CreateInputLayout(&layout[0], layout.size(), compiledBuffer, sizeof(compiledBuffer), &m_inputLayout)))
		{
			assert(0 && "CreateInputLayout失敗");
			Release();
			return false;
		}
	}

	//--------------------------------------------------
	// ピクセルシェーダ(_Color)
	//--------------------------------------------------
	{
		#include "KdPostProcess_Color_PS.inc"

		// 生成
		if (FAILED(D3D.GetDev()->CreatePixelShader(compiledBuffer, sizeof(compiledBuffer), nullptr, &m_colorPS)))
		{
			assert(0 && "ピクセルシェーダー作成失敗");
			Release();
			return false;
		}

	}

	//--------------------------------------------------
	// ピクセルシェーダ(_Blur)
	//--------------------------------------------------
	{
		#include "KdPostProcess_Blur_PS.inc"

		if (FAILED(D3D.GetDev()->CreatePixelShader(compiledBuffer, sizeof(compiledBuffer), nullptr, &m_blurPS)))
		{
			assert(0 && "ピクセルシェーダー作成失敗");
			Release();
			return false;
		}
	}

	//--------------------------------------------------
	// ピクセルシェーダ(_Blur)
	//--------------------------------------------------
	{
		#include "KdPostProcess_HBright_PS.inc"

		if (FAILED(D3D.GetDev()->CreatePixelShader(compiledBuffer, sizeof(compiledBuffer), nullptr, &m_HBrightPS)))
		{
			assert(0 && "ピクセルシェーダー作成失敗");
			Release();
			return false;
		}
	}

	//--------------------------------------------------
	// 定数バッファ
	//--------------------------------------------------
	m_cb0_Color.Create();
	m_cb0_Blur.Create();

	return true;
}

void KdPostProcessShader::Release()
{
	KdSafeRelease(m_VS);
	KdSafeRelease(m_inputLayout);
	KdSafeRelease(m_colorPS);
	m_cb0_Color.Release();
}

void KdPostProcessShader::ColorDraw(const KdTexture* a_pTex, const Math::Vector4& a_color)
{
	// テクスチャ確認
	if (a_pTex == nullptr) return;

	// Zバッファを無効化 現在のDepthStencilStateを記憶
	Microsoft::WRL::ComPtr<ID3D11DepthStencilState> saveDS;
	UINT saveStencilRef = 0;
	D3D.GetDevContext()->OMGetDepthStencilState(&saveDS, &saveStencilRef);
	D3D.GetDevContext()->OMSetDepthStencilState(SHADER.m_ds_ZDisable_ZWriteDisable, 0);

	// 定数バッファへ書き込み
	m_cb0_Color.Work().Color = a_color;	// 値の設定
	m_cb0_Color.Write();				// GPUへ転送
	D3D.GetDevContext()->VSSetConstantBuffers(0, 1, m_cb0_Color.GetAddress());
	D3D.GetDevContext()->PSSetConstantBuffers(0, 1, m_cb0_Color.GetAddress());

	// テクスチャ関係を設定
	D3D.GetDevContext()->PSSetShaderResources(0, 1, a_pTex->GetSRViewAddress());
	// サンプラを線形補完のClampモードへ遷移
	D3D.GetDevContext()->PSSetSamplers(0, 1, &SHADER.m_ss_Linear_Clamp);

	// シェーダー関連をセット
	D3D.GetDevContext()->VSSetShader(m_VS, 0, 0);
	D3D.GetDevContext()->IASetInputLayout(m_inputLayout);
	D3D.GetDevContext()->PSSetShader(m_colorPS, 0, 0);

	// 射影座標系に配置
	Vertex v[4] = {
		{ {-1,-1,0}, {0, 1} },// 0
		{ {-1, 1,0}, {0, 0} },// 1
		{ { 1,-1,0}, {1, 1} },// 2
		{ { 1, 1,0}, {1, 0} } // 3
	};

	// 描画
	D3D.DrawVertices(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLESTRIP, 4, &v[0], sizeof(Vertex));

	//
	ID3D11ShaderResourceView* nullSRV = nullptr;
	D3D.GetDevContext()->PSSetShaderResources(0, 1, &nullSRV);
	D3D.GetDevContext()->PSSetSamplers(0, 1, &SHADER.m_ss_Anisotropic_Wrap);
	D3D.GetDevContext()->OMSetDepthStencilState(saveDS.Get(), saveStencilRef);
}

void KdPostProcessShader::BlurDraw(const KdTexture* a_pTex, const Math::Vector2& a_dir)
{
	// テクスチャ確認
	if (a_pTex == nullptr) return;

	// 現在の設定を記憶
	Microsoft::WRL::ComPtr<ID3D11DepthStencilState> saveDS = {};
	UINT saveStencilRef = 0;
	D3D.GetDevContext()->OMGetDepthStencilState(&saveDS, &saveStencilRef);
	D3D.GetDevContext()->OMSetDepthStencilState(SHADER.m_ds_ZDisable_ZWriteDisable, 0);

	// テクスチャのサイズから1ピクセルのサイズを求める = テクセルサイズ
	Math::Vector2 ts = {};
	ts.x = 1.0f / a_pTex->GetWidth();
	ts.y = 1.0f / a_pTex->GetHeight();

	// ずれ分の座標を計算
	const int kMaxSampling = 31;
	float totalWeight = 0;
	const float kDispersion = 3;
	for (int i = 0; i < kMaxSampling; i++)
	{
		int pt = i - kMaxSampling / 2;
		m_cb0_Blur.Work().Offset[i].x = a_dir.x * (pt * ts.x);
		m_cb0_Blur.Work().Offset[i].y = a_dir.y * (pt * ts.y);

		// ガウス係数の計算
		float w = exp(-(pt * pt) / (2 * kDispersion * kDispersion));
		m_cb0_Blur.Work().Offset[i].z = w;

		// 合計を計算
		totalWeight += w;
	}

	// 係数を正規化
	for (int i = 0; i < 31; i++)
	{
		m_cb0_Blur.Work().Offset[i].z /= totalWeight;
	}

	// 定数バッファへ書き込み
	m_cb0_Blur.Write();


	D3D.GetDevContext()->VSSetConstantBuffers(0, 1, m_cb0_Blur.GetAddress());
	D3D.GetDevContext()->PSSetConstantBuffers(0, 1, m_cb0_Blur.GetAddress());

	D3D.GetDevContext()->PSSetShaderResources(0, 1, a_pTex->GetSRViewAddress());
	D3D.GetDevContext()->PSSetSamplers(0, 1, &SHADER.m_ss_Linear_Clamp);

	D3D.GetDevContext()->VSSetShader(m_VS, 0, 0);
	D3D.GetDevContext()->IASetInputLayout(m_inputLayout);
	D3D.GetDevContext()->PSSetShader(m_blurPS, 0, 0);

	Vertex v[4] = {
		{ {-1,-1,0}, {0, 1} },// 0
		{ {-1, 1,0}, {0, 0} },// 1
		{ { 1,-1,0}, {1, 1} },// 2
		{ { 1, 1,0}, {1, 0} } // 3
	};

	D3D.DrawVertices(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLESTRIP, 4, &v[0], sizeof(Vertex));

	ID3D11ShaderResourceView* nullSRV = nullptr;
	D3D.GetDevContext()->PSSetShaderResources(0, 1, &nullSRV);
	D3D.GetDevContext()->PSSetSamplers(0, 1, &SHADER.m_ss_Anisotropic_Wrap);
	D3D.GetDevContext()->OMSetDepthStencilState(saveDS.Get(), saveStencilRef);
}

void KdPostProcessShader::GenerateBlur(KdBlurTexture& a_pBlurTex, const KdTexture* a_pSrcTex)
{
	//
	RestoreRenderTarget rrt = {};

	//
	D3D11_VIEWPORT saveVP = {};
	UINT numVP = 1;
	D3D.GetDevContext()->RSGetViewports(&numVP, &saveVP);

	//
	D3D11_VIEWPORT vp = {};

	for (int i = 0; i < 5; i++)
	{
		D3D.GetDevContext()->OMSetRenderTargets(1, a_pBlurTex.m_rt[i][0]->GetRTViewAddress(), nullptr);

		vp = D3D11_VIEWPORT{ 0.0f, 0.0f, (float)a_pBlurTex.m_rt[i][0]->GetWidth(),(float)a_pBlurTex.m_rt[i][0]->GetHeight(), 0.0f, 1.0f };
		D3D.GetDevContext()->RSSetViewports(1, &vp);

		if (i == 0)
		{
			ColorDraw(a_pSrcTex);
		}
		else
		{
			ColorDraw(a_pBlurTex.m_rt[i - 1][0].get());
		}

		D3D.GetDevContext()->OMSetRenderTargets(1, a_pBlurTex.m_rt[i][1]->GetRTViewAddress(), nullptr);

		BlurDraw(a_pBlurTex.m_rt[i][0].get(), { 1, 0 });

		D3D.GetDevContext()->OMSetRenderTargets(1, a_pBlurTex.m_rt[i][0]->GetRTViewAddress(), nullptr);

		BlurDraw(a_pBlurTex.m_rt[i][1].get(), { 0, 1 });
	}

	D3D.GetDevContext()->RSSetViewports(1, &saveVP);
}

void KdPostProcessShader::BrightFiltering(const KdTexture* a_pDestRT, const KdTexture* a_pSrcTex)
{
	RestoreRenderTarget rrt = {};

	D3D11_VIEWPORT saveVP = {};
	UINT numVP = 1;
	D3D.GetDevContext()->RSGetViewports(&numVP, &saveVP);

	D3D.GetDevContext()->OMSetRenderTargets(1, a_pDestRT->GetRTViewAddress(), nullptr);

	D3D11_VIEWPORT vp = { 0.0f, 0.0f,(float)a_pDestRT->GetWidth(), (float)a_pDestRT->GetHeight(), 0.0f, 1.0f };
	D3D.GetDevContext()->RSSetViewports(1, &vp);

	ID3D11DepthStencilState* saveDS = {};
	UINT saveStencilRef = 0;
	D3D.GetDevContext()->OMGetDepthStencilState(&saveDS, &saveStencilRef);

	D3D.GetDevContext()->OMSetDepthStencilState(SHADER.m_ds_ZDisable_ZWriteDisable, 0);

	D3D.GetDevContext()->PSSetShaderResources(0, 1, a_pSrcTex->GetSRViewAddress());
	D3D.GetDevContext()->PSSetSamplers(0, 1, &SHADER.m_ss_Linear_Clamp);

	D3D.GetDevContext()->VSSetShader(m_VS, 0, 0);
	D3D.GetDevContext()->IASetInputLayout(m_inputLayout);
	D3D.GetDevContext()->PSSetShader(m_HBrightPS, 0, 0);

	Vertex v[4] = {
		{ {-1,-1,0}, {0, 1} },// 0
		{ {-1, 1,0}, {0, 0} },// 1
		{ { 1,-1,0}, {1, 1} },// 2
		{ { 1, 1,0}, {1, 0} } // 3
	};

	D3D.DrawVertices(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLESTRIP, 4, &v[0], sizeof(Vertex));

	ID3D11ShaderResourceView* nullSRV = nullptr;
	D3D.GetDevContext()->PSSetShaderResources(0, 1, &nullSRV);

	D3D.GetDevContext()->PSSetSamplers(0, 1, &SHADER.m_ss_Anisotropic_Wrap);
	D3D.GetDevContext()->OMSetDepthStencilState(saveDS, saveStencilRef);
	saveDS->Release();

	D3D.GetDevContext()->RSSetViewports(1, &saveVP);
}