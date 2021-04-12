//-----------------------------------------------------------------------------
// File: KdShaderManager.cpp
//
// Edit: 2021/03/10 野筒隼輔
//-----------------------------------------------------------------------------
#include "System/KdSystem.h"
#include "KdShaderManager.h"

//-----------------------------------------------------------------------------
// Name: KdShaderManager()
// Desc: コンストラクタ
//-----------------------------------------------------------------------------
KdShaderManager::KdShaderManager()
	: m_standardShader()
	, m_effectShader()
	, m_spriteShader()
	, m_modelShader()
	, m_genShadowMapShader()
	, m_postProcessShader()
	, m_ds_ZEnable_ZWriteEnable(nullptr)
	, m_ds_ZDisable_ZWriteDisable(nullptr)
	, m_ds_ZEnable_ZWriteDisable(nullptr)
	, m_rs_CullBack(nullptr)
	, m_rs_CullNone(nullptr)
	, m_bs_Alpha(nullptr)
	, m_bs_Add(nullptr)
	, m_ss_Anisotropic_Wrap(nullptr)
	, m_ss_Anisotropic_Clamp(nullptr)
	, m_ss_Linear_Clamp(nullptr)
	, m_ss_ShadowMapComparison(nullptr)
{
}

//-----------------------------------------------------------------------------
// Name: Init()
// Desc: 初期化
//-----------------------------------------------------------------------------
void KdShaderManager::Init()
{
	//--------------------------------------------------
	// シェーダ
	//--------------------------------------------------
	// 基本シェーダ
	m_standardShader.Init();
	// エフェクトシェーダ
	m_effectShader.Init();
	// 2Dスプライトシェーダ
	m_spriteShader.Init();
	// 物理ベースによる3Dモデル描画シェーダー
	m_modelShader.Init();
	// シャドウマップ生成シェーダー
	m_genShadowMapShader.Init();
	// ポスト関連シェーダ
	m_postProcessShader.Init();

	//--------------------------------------------------
	// 定数バッファ
	//--------------------------------------------------

	// カメラ
	m_cb7_Camera.Create();
	D3D.GetDevContext()->VSSetConstantBuffers(7, 1, m_cb7_Camera.GetAddress());
	D3D.GetDevContext()->PSSetConstantBuffers(7, 1, m_cb7_Camera.GetAddress());

	// 初期ビュー行列
	m_cb7_Camera.Work().mV = Math::Matrix::CreateTranslation(0, 0, -5);
	m_cb7_Camera.Work().CamPos = m_cb7_Camera.Work().mV.Translation();
	m_cb7_Camera.Work().mV = m_cb7_Camera.Work().mV.Invert();

	// 初期射影行列
	m_cb7_Camera.Work().mP = DirectX::XMMatrixPerspectiveFovLH(DirectX::XMConvertToRadians(60), D3D.GetBackBuffer()->GetAspectRatio(), 0.01f, 1000);
	m_cb7_Camera.Write();

	// ライト
	m_cb8_Light.Create();
	m_cb8_Light.Work().DL_Dir.Normalize();
	m_cb8_Light.Write();
	D3D.GetDevContext()->VSSetConstantBuffers(8, 1, m_cb8_Light.GetAddress());
	D3D.GetDevContext()->PSSetConstantBuffers(8, 1, m_cb8_Light.GetAddress());

	//--------------------------------------------------
	// パイプラインステート
	//--------------------------------------------------

	// 深度ステンシルステート作成
	m_ds_ZEnable_ZWriteEnable	= D3D.CreateDepthStencilState(true, true);
	m_ds_ZDisable_ZWriteDisable = D3D.CreateDepthStencilState(false, false);
	m_ds_ZEnable_ZWriteDisable	= D3D.CreateDepthStencilState(true, false);

	// ラスタライザーステート作成
	m_rs_CullBack = D3D.CreateRasterizerState(D3D11_CULL_BACK, D3D11_FILL_SOLID, true, false);
	m_rs_CullNone = D3D.CreateRasterizerState(D3D11_CULL_NONE, D3D11_FILL_SOLID, true, false);

	// ブレンドステート作成
	m_bs_Alpha	= D3D.CreateBlendState(KdBlendMode::Alpha);
	m_bs_Add	= D3D.CreateBlendState(KdBlendMode::Add);

	// サンプラーステート
	m_ss_Anisotropic_Wrap	 = D3D.CreateSamplerState(KdSamplerFilterMode::Anisotropic, 4, KdSamplerAddressingMode::Wrap, false);
	m_ss_Anisotropic_Clamp	 = D3D.CreateSamplerState(KdSamplerFilterMode::Anisotropic, 4, KdSamplerAddressingMode::Clamp, false);
	m_ss_Linear_Clamp		 = D3D.CreateSamplerState(KdSamplerFilterMode::Linear, 4, KdSamplerAddressingMode::Clamp, false);
	m_ss_ShadowMapComparison = D3D.CreateSamplerState(KdSamplerFilterMode::Linear, 4, KdSamplerAddressingMode::Clamp, true);

	D3D.GetDevContext()->PSSetSamplers( 0, 1, &m_ss_Anisotropic_Wrap);
	D3D.GetDevContext()->PSSetSamplers(10, 1, &m_ss_ShadowMapComparison);
}

//-----------------------------------------------------------------------------
// Name: Release()
// Desc: 解放
//-----------------------------------------------------------------------------
void KdShaderManager::Release()
{
	// 各シェーダ解放
	m_modelShader.Release();
	m_standardShader.Release();
	m_effectShader.Release();
	m_spriteShader.Release();
	m_genShadowMapShader.Release();
	m_postProcessShader.Release();

	// カメラ, ライト解放
	m_cb7_Camera.Release();
	m_cb8_Light.Release();

	// 深度ステンシルステート解放
	KdSafeRelease(m_ds_ZEnable_ZWriteEnable);
	KdSafeRelease(m_ds_ZDisable_ZWriteDisable);
	KdSafeRelease(m_ds_ZEnable_ZWriteDisable);

	// ラスタライザーステート解放
	KdSafeRelease(m_rs_CullBack);
	KdSafeRelease(m_rs_CullNone);

	// ブレンドステート解放
	KdSafeRelease(m_bs_Alpha);
	KdSafeRelease(m_bs_Add);

	// サンプラーステート解放
	KdSafeRelease(m_ss_Anisotropic_Wrap);
	KdSafeRelease(m_ss_Anisotropic_Clamp);
	KdSafeRelease(m_ss_Linear_Clamp);
	KdSafeRelease(m_ss_ShadowMapComparison);
}

//-----------------------------------------------------------------------------
// Name: AddPointLight()
// Desc: 点光の追加
//-----------------------------------------------------------------------------
void KdShaderManager::AddPointLight(const KdVector3& pos, float radius, const KdVector3& color)
{
	int idx = m_cb8_Light.GetWork().PL_Cnt;
	if (idx < (int)m_cb8_Light.GetWork().PL.size()) {
		m_cb8_Light.Work().PL[idx].Pos = pos;
		m_cb8_Light.Work().PL[idx].Radius = radius;
		m_cb8_Light.Work().PL[idx].Color = color;

		m_cb8_Light.Work().PL_Cnt++;
	}
}

//-----------------------------------------------------------------------------
// Name: ResetPointLight()
// Desc: 点光のリセット ※全消し
//-----------------------------------------------------------------------------
void KdShaderManager::ResetPointLight()
{
	m_cb8_Light.Work().PL_Cnt = 0;
}