//-----------------------------------------------------------------------------
// File: KdGenerateShadowMapShader.cpp
//
// Edit: 2021/02/17 野筒隼輔
//-----------------------------------------------------------------------------
#include "KdGenerateShadowMapShader.h"
#include "System/KdSystem.h"
#include "Application/Component/CameraComponent.h"
#include "Application/Manager/CameraManager.h"
#include "Application/Manager/DebugManager.h"

//-----------------------------------------------------------------------------
// Name: 
// Desc: 
//-----------------------------------------------------------------------------
KdGenerateShadowMapShader::KdGenerateShadowMapShader()
	: m_VS(nullptr)
	, m_inputLayout(nullptr)
	, m_PS(nullptr)
	, m_dirLightShadowMap(nullptr)
	, m_dirLightZBuffer(nullptr)
	, m_saveRT(nullptr)
	, m_saveZ(nullptr)
	, m_numVP(1)
	, m_saveVP()
	, m_lightCameraLook(KdVector3(0.5f, -1.0f, 0.5f))
	, m_cb0()
{
}

//-----------------------------------------------------------------------------
// Name: 
// Desc: 
//-----------------------------------------------------------------------------
bool KdGenerateShadowMapShader::Init()
{
	//-------------------------------------
	// 頂点シェーダ
	//-------------------------------------
	{
		// コンパイル済みのシェーダーヘッダーファイルをインクルード
		#include "KdGenerateShadowMapShader_VS.inc"

		// 頂点シェーダー作成
		if (FAILED(D3D.GetDev()->CreateVertexShader(compiledBuffer, sizeof(compiledBuffer), nullptr, &m_VS)))
		{
			assert(0 && "頂点シェーダー作成失敗");
			Release();
			return false;
		}

		// １頂点の詳細な情報
		std::vector<D3D11_INPUT_ELEMENT_DESC> layout = {
			{ "POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT,		0,  0, D3D11_INPUT_PER_VERTEX_DATA, 0 },
			{ "TEXCOORD", 0, DXGI_FORMAT_R32G32_FLOAT,			0, 12, D3D11_INPUT_PER_VERTEX_DATA, 0 },// 引数の12...R(4)G(4)B(4) * 1 = 12
			{ "NORMAL",   0, DXGI_FORMAT_R32G32B32_FLOAT,		0, 20, D3D11_INPUT_PER_VERTEX_DATA, 0 },// C++とHLSLとではメモリが違うのでこういうことをしなければならない
			{ "COLOR",    0, DXGI_FORMAT_R8G8B8A8_UNORM,		0, 32, D3D11_INPUT_PER_VERTEX_DATA, 0 },
			{ "TANGENT",  0, DXGI_FORMAT_R32G32B32_FLOAT,		0, 36, D3D11_INPUT_PER_VERTEX_DATA, 0 },
		};

		// 頂点入力レイアウト作成
		if (FAILED(D3D.GetDev()->CreateInputLayout(&layout[0], layout.size(), &compiledBuffer[0], sizeof(compiledBuffer), &m_inputLayout)))
		{
			assert(0 && "CreateInputLayout作成失敗");
			Release();
			return false;
		}
	}

	//-------------------------------------
	// ピクセルシェーダ
	//-------------------------------------
	{
		// コンパイル済みのシェーダーヘッダーファイルをインクルード
		#include "KdGenerateShadowMapShader_PS.inc"

		if (FAILED(D3D.GetDev()->CreatePixelShader(compiledBuffer, sizeof(compiledBuffer), nullptr, &m_PS)))
		{
			assert(0 && "ピクセルシェーダー作成失敗");
			Release();
			return false;
		}
	}

	//-------------------------------------
	// 定数バッファ作成
	//-------------------------------------
	m_cb0.Create();

	//-------------------------------------
	// 深度マップやZバッファを作成
	//-------------------------------------
	constexpr int size = 10240;// 1024x ※余裕をもって！
	m_dirLightShadowMap = std::make_shared<KdTexture>();
	m_dirLightShadowMap->CreateRenderTarget(size, size, DXGI_FORMAT_R32_FLOAT);
	m_dirLightZBuffer = std::make_shared<KdTexture>();
	m_dirLightZBuffer->CreateDepthStencil(size, size, DXGI_FORMAT_R32_TYPELESS);

	return true;
}

//-----------------------------------------------------------------------------
// Name: 
// Desc: 
//-----------------------------------------------------------------------------
void KdGenerateShadowMapShader::Release()
{
	KdSafeRelease(m_VS);
	KdSafeRelease(m_inputLayout);
	KdSafeRelease(m_PS);
	m_cb0.Release();
}

//-----------------------------------------------------------------------------
// Name: 
// Desc: 
//-----------------------------------------------------------------------------
void KdGenerateShadowMapShader::Begin()
{
	if ((m_saveRT) || (m_saveZ)) { return; }

	//-----------------------------------------------------
	// 記憶
	//-----------------------------------------------------
	// RenderTarget Zバッファ
	D3D.GetDevContext()->OMGetRenderTargets(1, &m_saveRT, &m_saveZ);
	// ビューポート
	D3D.GetDevContext()->RSGetViewports(&m_numVP, &m_saveVP);

	//-----------------------------------------------------
	// 変更
	//-----------------------------------------------------
	// RenderTarget Zバッファ
	D3D.GetDevContext()->OMSetRenderTargets(1, m_dirLightShadowMap->GetRTViewAddress(), m_dirLightZBuffer->GetDSView());
	// ビューポート
	D3D11_VIEWPORT vp = { 0, 0, static_cast<FLOAT>(m_dirLightShadowMap->GetWidth()), static_cast<FLOAT>(m_dirLightShadowMap->GetHeight()), 0, 1 };
	D3D.GetDevContext()->RSSetViewports(1, &vp);

	//-----------------------------------------------------
	// クリア.設定
	//-----------------------------------------------------
	// クリア
	D3D.GetDevContext()->ClearRenderTargetView(m_dirLightShadowMap->GetRTView(), Math::Color(1, 1, 1, 1));
	D3D.GetDevContext()->ClearDepthStencilView(m_dirLightZBuffer->GetDSView(), D3D11_CLEAR_DEPTH, 1, 0);

	// ライトカメラ設定
	SettingLightCamera();
	// シェーダーや定数バッファをセット
	SetToDevice();
}

//-----------------------------------------------------------------------------
// Name: 
// Desc: 
//-----------------------------------------------------------------------------
void KdGenerateShadowMapShader::End()
{
	if ((!m_saveRT) || (!m_saveZ)) { return; }

	//-----------------------------------------------------
	// 復元
	//-----------------------------------------------------
	// RenderTarget Zバッファ
	D3D.GetDevContext()->OMSetRenderTargets(1, &m_saveRT, m_saveZ);
	// ビューポート
	D3D.GetDevContext()->RSSetViewports(1, &m_saveVP);

	//-----------------------------------------------------
	// 保存用情報の解放
	//-----------------------------------------------------
	KdSafeRelease(m_saveRT);
	KdSafeRelease(m_saveZ);
}

//-----------------------------------------------------------------------------
// Name: 
// Desc: 
//-----------------------------------------------------------------------------
void KdGenerateShadowMapShader::SetToDevice()
{
	// 頂点シェーダをセット
	D3D.GetDevContext()->VSSetShader(m_VS, 0, 0);
	// 頂点レイアウトをセット
	D3D.GetDevContext()->IASetInputLayout(m_inputLayout);
	// ピクセルシェーダをセット
	D3D.GetDevContext()->PSSetShader(m_PS, 0, 0);

	//-----------------------------------------------------
	// 定数バッファをセット
	//-----------------------------------------------------
	// オブジェクト定数バッファ
	D3D.GetDevContext()->VSSetConstantBuffers(0, 1, m_cb0.GetAddress());
	D3D.GetDevContext()->PSSetConstantBuffers(0, 1, m_cb0.GetAddress());
}

//-----------------------------------------------------------------------------
// Name: 
// Desc: 
//-----------------------------------------------------------------------------
void KdGenerateShadowMapShader::DrawMeshDepth(const KdMesh* mesh, const std::vector<KdMaterial>& materials)
{
	if (mesh == nullptr) { return; }

	// 定数バッファ書き込み
	m_cb0.Write();

	// メッシュ情報をセット
	mesh->SetToDevice();

	// 全サブセット
	for (UINT subi = 0; subi < mesh->GetSubsets().size(); subi++)
	{
		// 面が１枚も無い場合はスキップ
		if (mesh->GetSubsets()[subi].FaceCount == 0) { continue; }

		// マテリアル
		const KdMaterial& material = materials[mesh->GetSubsets()[subi].MaterialNo];

		//-----------------------------------------------------
		// テクスチャセット
		//-----------------------------------------------------
		D3D.GetDevContext()->PSSetShaderResources(0, 1, material.BaseColorTex->GetSRViewAddress());

		//-----------------------------------------------------
		// サブセット描画
		//-----------------------------------------------------
		mesh->DrawSubset(subi);
	}
}

//-----------------------------------------------------------------------------
// Name: SettingLightCamera()
// Desc: ライトカメラ設定
//-----------------------------------------------------------------------------
void KdGenerateShadowMapShader::SettingLightCamera()
{
	KdMatrix viewMat = {}, projMat = {};

	//-----------------------------------------------------
	// カメラ情報からライト座標に反映
	//-----------------------------------------------------
	std::shared_ptr<CameraComponent> camera = CAMERA_MAGER.GetTargetCamera().lock();
	KdMatrix cameraMatrix = {};
	if (camera)
	{
		const KdMatrix& cameraMat = camera->GetCameraMatrix();
		cameraMatrix.CreateTranslation(cameraMat.GetTranslation());

		KdVector3 move = cameraMat.GetAxisZ();
		move.y = 10.0f;
		move.Normalize();

		move *= 20.0f;// カメラのZ軸に20.0f移動
		cameraMatrix.Move(move);
	}

	//-----------------------------------------------------
	// ライトのビュー行列を作成
	//-----------------------------------------------------
	// m_shadowLookの方向に向ける
	viewMat.LookTo(m_lightCameraLook, KdVector3::Up());
	// 座標設定
	KdMatrix transMat = {};
	transMat.CreateTranslation(0.0f, 0.0f, -40.0f);
	viewMat = transMat * viewMat;
	viewMat *= cameraMatrix;
	viewMat.Inverse();

	//-----------------------------------------------------
	// ライトの正射影行列を作成
	//-----------------------------------------------------
	projMat.CreateProjection_Orthographic(200.0f, 200.0f, 0.0f, 300.0f);

	//-----------------------------------------------------
	// 最終設定
	//-----------------------------------------------------
	// ビュー*射影をセット
	m_cb0.Work().mLightVP = viewMat * projMat;
	// KdModelShaderで使用するためライトの定数バッファに行列を入れておく
	SHADER.m_cb8_Light.Work().DL_mLightVP = viewMat * projMat;
}