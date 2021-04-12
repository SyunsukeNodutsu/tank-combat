//-----------------------------------------------------------------------------
// File: DrawManager.cpp
//
// Edit: 2021/02/11 野筒隼輔
//-----------------------------------------------------------------------------
#include "DrawManager.h"
#include "ClearManager.h"
#include "ObjectManager.h"
#include "Application/Game/Other/Radio.h"
#include "Application/Game/Scene/InterruptCheck.h"
#include "Application/Game/Scene/Pause.h"
#include "Application/Game/Scene/Scene.h"

//-----------------------------------------------------------------------------
// Name: DrawManager()
// Desc: コンストラクタ
//-----------------------------------------------------------------------------
DrawManager::DrawManager()
	: m_debugLines()
	, m_spScreenRT(nullptr)
	, m_spScreenZ(nullptr)
	, m_spHeightBrightTex(nullptr)
	, m_blurTex()
	, m_blurValue(Math::Vector2(0.0f, 0.0f))
{
}

//-----------------------------------------------------------------------------
// Name: Initialize()
// Desc: 初期化
//-----------------------------------------------------------------------------
void DrawManager::Initialize()
{
	// 解像度
	constexpr int height = 720;
	constexpr int width = 1280;

	// DXGI_FORMAT_R16G16B16A16_FLOAT ... HDR対応 ※重たいので多用厳禁
	m_spScreenRT = std::make_shared<KdTexture>();
	m_spScreenRT->CreateRenderTarget(width, height, DXGI_FORMAT_R16G16B16A16_FLOAT);

	m_spScreenZ = std::make_shared<KdTexture>();
	m_spScreenZ->CreateDepthStencil(width, height);

	m_spHeightBrightTex = std::make_shared<KdTexture>();
	m_spHeightBrightTex->CreateRenderTarget(width, height, DXGI_FORMAT_R16G16B16A16_FLOAT);

	m_blurTex.Create(width, height);
}

//-----------------------------------------------------------------------------
// Name: Draw()
// Desc: 描画を一元管理
//-----------------------------------------------------------------------------
void DrawManager::Draw()
{
	//--------------------------------------------------
	// ぼかし描画 ※ボックス(ブラー)
	//--------------------------------------------------
	{
		// レンダーターゲット設定
		RestoreRenderTarget rrt = {};
		D3D.GetDevContext()->OMSetRenderTargets(1, m_spScreenRT->GetRTViewAddress(), m_spScreenZ->GetDSView());
		D3D.GetDevContext()->ClearRenderTargetView(m_spScreenRT->GetRTView(), kBlackColor);
		D3D.GetDevContext()->ClearDepthStencilView(m_spScreenZ->GetDSView(), D3D11_CLEAR_DEPTH | D3D11_CLEAR_STENCIL, 1, 0);

		// シャドウマップ生成描画
		DrawShadowMap();

		// 不透明物描画
		DrawOpaque();

		// 半透明物描画
		DrawTranslucent();

		// Effekseer描画
		EFFEKSEER.Update();
	}

	// ぼかし描画 ※Debug用のぼかしを強めるフラグを確認
	SHADER.m_postProcessShader.BlurDraw(m_spScreenRT.get(), m_blurValue);

	//--------------------------------------------------
	// ぼかし描画 ※高輝度抽出(ブルーム)
	//--------------------------------------------------
	if (true)// ここフラグで管理したい
	{
		// しきい値以上のピクセルを抽出
		SHADER.m_postProcessShader.BrightFiltering(m_spHeightBrightTex.get(), m_spScreenRT.get());

		// 一定以上の明るさを持ったテクスチャを各サイズぼかし画像作成
		SHADER.m_postProcessShader.GenerateBlur(m_blurTex, m_spHeightBrightTex.get());

		// 加算合成に変更
		D3D.GetDevContext()->OMSetBlendState(SHADER.m_bs_Add, Math::Color(0, 0, 0, 0), 0xFFFFFFFF);

		// 各サイズの画像を加算合成
		for (int bCnt = 0; bCnt < 5; bCnt++)
		{
			SHADER.m_postProcessShader.ColorDraw(m_blurTex.m_rt[bCnt][0].get(), DirectX::SimpleMath::Vector4(1, 1, 1, 1));
		}

		// 合成方法をもとに戻す
		D3D.GetDevContext()->OMSetBlendState(SHADER.m_bs_Alpha, Math::Color(0, 0, 0, 0), 0xFFFFFFFF);
	}

	//--------------------------------------------------
	// ぼかさない描画
	//--------------------------------------------------

	// 2D描画
	DrawSprite();

	// デバッグライン描画
	DrawDebugLines();
}

//-----------------------------------------------------------------------------
// Name: DrawShadowMap()
// Desc: シャドウマップ描画
//-----------------------------------------------------------------------------
void DrawManager::DrawShadowMap()
{
	SHADER.m_genShadowMapShader.Begin();

	for (auto& object : OBJ_MAGER.GetObjectList()) {
		object->DrawShadowMap();
	}

	SHADER.m_genShadowMapShader.End();
}

//-----------------------------------------------------------------------------
// Name: DrawOpaque()
// Desc: 不透明物描画
//-----------------------------------------------------------------------------
void DrawManager::DrawOpaque()
{
	// ライトの情報をセット
	SHADER.m_cb8_Light.Write();
	// シャドウマップをセット
	D3D.GetDevContext()->PSSetShaderResources(102, 1, SHADER.m_genShadowMapShader.GetDirShadowMap()->GetSRViewAddress());
	// ModelShader使用
	SHADER.m_modelShader.SetToDevice();

	for (auto& object : OBJ_MAGER.GetObjectList()) {
		object->Draw();
	}

	// シャドウマップを解除
	ID3D11ShaderResourceView* nullSRV = nullptr;
	D3D.GetDevContext()->PSSetShaderResources(102, 1, &nullSRV);
}

//-----------------------------------------------------------------------------
// Name: DrawTranslucent()
// Desc: 透明物描画(Effect描画)
//-----------------------------------------------------------------------------
void DrawManager::DrawTranslucent()
{
	// EffectShader使用
	SHADER.m_effectShader.SetToDevice();

	// 描画オプション
	D3D.GetDevContext()->OMSetDepthStencilState(SHADER.m_ds_ZEnable_ZWriteDisable, 0);	// Z情報は使うが Z書き込みOFF
	D3D.GetDevContext()->RSSetState(SHADER.m_rs_CullNone);								// カリングなし ※両面描画

	for (auto& object : OBJ_MAGER.GetObjectList()) {
		object->DrawEffect();
	}

	// 描画オプション
	D3D.GetDevContext()->OMSetDepthStencilState(SHADER.m_ds_ZEnable_ZWriteEnable, 0);	// Z書き込みON
	D3D.GetDevContext()->RSSetState(SHADER.m_rs_CullBack);								// カリングあり ※表面のみ描画
}

//-----------------------------------------------------------------------------
// Name: DrawSprite()
// Desc: 2D描画
//-----------------------------------------------------------------------------
void DrawManager::DrawSprite()
{
	if (SCENE.IsEdiorMode()) { return; }

	SHADER.m_spriteShader.Begin();

	// 登場するObject達2D描画
	for (auto& object : OBJ_MAGER.GetObjectList()) {
		object->DrawSprite();
	}

	// 無線
	RADIO.DrawSprite();
	// 一時停止管理
	PAUSE.DrawSprite();
	// クリア管理
	CLEAR_MAGER.DrawSprite();
	// 割り込み確認
	IRQ.DrawSprite();

	SHADER.m_spriteShader.End();
}

//-----------------------------------------------------------------------------
// Name: DrawDebugLines()
// Desc: デバッグライン描画
//-----------------------------------------------------------------------------
void DrawManager::DrawDebugLines()
{
	// EffectShader使用
	SHADER.m_effectShader.SetToDevice();
	SHADER.m_effectShader.SetTexture(D3D.GetWhiteTex()->GetSRView());

	// Zバッファ使用OFF,書き込みON
	D3D.GetDevContext()->OMSetDepthStencilState(SHADER.m_ds_ZDisable_ZWriteDisable, 0);

	// debugLinesの中身があるとき
	if (m_debugLines.size() >= 1)
	{
		SHADER.m_effectShader.SetWorldMatrix(Math::Matrix());
		SHADER.m_effectShader.DrawVertices(m_debugLines, D3D_PRIMITIVE_TOPOLOGY_LINELIST);
		m_debugLines.clear();
	}

	// バッファ使用OFF,書き込みON(デフォルトに戻す)
	D3D.GetDevContext()->OMSetDepthStencilState(SHADER.m_ds_ZEnable_ZWriteEnable, 0);
}



//=============================================================================
//
// DebugLine
//
//=============================================================================

//-----------------------------------------------------------------------------
// Name: AddDebugLine()
// Desc: デバッグライン追加
//-----------------------------------------------------------------------------
void DrawManager::AddDebugLine(const Math::Vector3& a_pos01, const Math::Vector3& a_pos02, const Math::Color& a_color)
{
	KdEffectShader::Vertex ver = {};

	// ラインの開始頂点
	ver.Color = a_color;
	ver.UV = { 0.0f, 0.0f };
	ver.Pos = a_pos01;
	m_debugLines.push_back(ver);

	// ラインの終了頂点
	ver.Pos = a_pos02;
	m_debugLines.push_back(ver);
}

//-----------------------------------------------------------------------------
// Name: AddDebugSphereLine()
// Desc: デバッグ球追加
//-----------------------------------------------------------------------------
void DrawManager::AddDebugSphereLine(const Math::Vector3& a_pos, const float a_radius, const Math::Color& a_color)
{
	KdEffectShader::Vertex ver = {};
	ver.Color = a_color;
	ver.UV = { 0.0f,0.0f };

	constexpr int detail = 32;// 目の錯覚の関係で16くらいで十分
	for (UINT i = 0; i < detail + 1; ++i)
	{
		// XZ平面
		ver.Pos = a_pos;
		ver.Pos.x += cosf(i * (360 / detail) * KdToRadians) * a_radius;
		ver.Pos.z += sinf(i * (360 / detail) * KdToRadians) * a_radius;
		m_debugLines.push_back(ver);

		ver.Pos = a_pos;
		ver.Pos.x += cosf((i + 1) * (360 / detail) * KdToRadians) * a_radius;
		ver.Pos.z += sinf((i + 1) * (360 / detail) * KdToRadians) * a_radius;
		m_debugLines.push_back(ver);

		// XY平面
		ver.Pos = a_pos;
		ver.Pos.x += cosf(i * (360 / detail) * KdToRadians) * a_radius;
		ver.Pos.y += sinf(i * (360 / detail) * KdToRadians) * a_radius;
		m_debugLines.push_back(ver);

		ver.Pos = a_pos;
		ver.Pos.x += cosf((i + 1) * (360 / detail) * KdToRadians) * a_radius;
		ver.Pos.y += sinf((i + 1) * (360 / detail) * KdToRadians) * a_radius;
		m_debugLines.push_back(ver);

		// YZ平面
		ver.Pos = a_pos;
		ver.Pos.y += cosf(i * (360 / detail) * KdToRadians) * a_radius;
		ver.Pos.z += sinf(i * (360 / detail) * KdToRadians) * a_radius;
		m_debugLines.push_back(ver);

		ver.Pos = a_pos;
		ver.Pos.y += cosf((i + 1) * (360 / detail) * KdToRadians) * a_radius;
		ver.Pos.z += sinf((i + 1) * (360 / detail) * KdToRadians) * a_radius;
		m_debugLines.push_back(ver);
	}
}