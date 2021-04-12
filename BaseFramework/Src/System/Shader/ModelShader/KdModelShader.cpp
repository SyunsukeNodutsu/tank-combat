//-----------------------------------------------------------------------------
// File: KdModelShader.cpp
//
// Edit: 2021/03/27 野筒隼輔
//-----------------------------------------------------------------------------
#include "System/KdSystem.h"
#include "KdModelShader.h"

//-----------------------------------------------------------------------------
// Name: KdModelShader()
// Desc: コンストラクタ
//-----------------------------------------------------------------------------
KdModelShader::KdModelShader()
	: m_VS(nullptr)
	, m_inputLayout(nullptr)
	, m_PS(nullptr)
	, m_cb0()
	, m_cb1_Material()
{
}

//-----------------------------------------------------------------------------
// Name: Init()
// Desc: 初期化
//-----------------------------------------------------------------------------
bool KdModelShader::Init()
{
	//-------------------------------------
	// 頂点シェーダ
	//-------------------------------------
	{
		// コンパイル済みのシェーダーヘッダーファイルをインクルード
		#include "KdModelShader_VS.inc"

		// 頂点シェーダー作成
		if (FAILED(D3D.GetDev()->CreateVertexShader(compiledBuffer, sizeof(compiledBuffer), nullptr, &m_VS))) {
			assert(0 && "頂点シェーダー作成失敗");
			Release();
			return false;
		}

		// １頂点の詳細な情報
		// 第５引数 ... 前の要素の直後で現在の要素を定義するには、D3D11_APPEND_ALIGNED_ELEMENT を使用すると便利
		std::vector<D3D11_INPUT_ELEMENT_DESC> layout = {
			{ "POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT,		0,  0, D3D11_INPUT_PER_VERTEX_DATA, 0 },
			{ "TEXCOORD", 0, DXGI_FORMAT_R32G32_FLOAT,			0, 12, D3D11_INPUT_PER_VERTEX_DATA, 0 },
			{ "NORMAL",   0, DXGI_FORMAT_R32G32B32_FLOAT,		0, 20, D3D11_INPUT_PER_VERTEX_DATA, 0 },
			{ "COLOR",    0, DXGI_FORMAT_R8G8B8A8_UNORM,		0, 32, D3D11_INPUT_PER_VERTEX_DATA, 0 },
			{ "TANGENT",  0, DXGI_FORMAT_R32G32B32_FLOAT,		0, 36, D3D11_INPUT_PER_VERTEX_DATA, 0 },
		};

		// 頂点入力レイアウト作成
		if (FAILED(D3D.GetDev()->CreateInputLayout(&layout[0], layout.size(), &compiledBuffer[0], sizeof(compiledBuffer), &m_inputLayout))) {
			assert(0 && "CreateInputLayout失敗");
			Release();
			return false;
		}
	}

	//-------------------------------------
	// ピクセルシェーダ
	//-------------------------------------
	{
		// コンパイル済みのシェーダーヘッダーファイルをインクルード
		#include "KdModelShader_PS.inc"

		if (FAILED(D3D.GetDev()->CreatePixelShader(compiledBuffer, sizeof(compiledBuffer), nullptr, &m_PS))) {
			assert(0 && "ピクセルシェーダー作成失敗");
			Release();
			return false;
		}
	}

	//-------------------------------------
	// 定数バッファ作成
	//-------------------------------------
	m_cb0.Create();
	m_cb1_Material.Create();

	return true;
}

//-----------------------------------------------------------------------------
// Name: Release()
// Desc: 解放
//-----------------------------------------------------------------------------
void KdModelShader::Release()
{
	KdSafeRelease(m_VS);
	KdSafeRelease(m_PS);
	KdSafeRelease(m_inputLayout);
	m_cb0.Release();
	m_cb1_Material.Release();
}

//-----------------------------------------------------------------------------
// Name: SetToDevice()
// Desc: GPUに転送
//-----------------------------------------------------------------------------
void KdModelShader::SetToDevice()
{
	// 頂点シェーダをセット
	D3D.GetDevContext()->VSSetShader(m_VS, 0, 0);
	// 頂点レイアウトをセット
	D3D.GetDevContext()->IASetInputLayout(m_inputLayout);

	// ピクセルシェーダをセット
	D3D.GetDevContext()->PSSetShader(m_PS, 0, 0);

	// 定数バッファをセット
	D3D.GetDevContext()->VSSetConstantBuffers(0, 1, m_cb0.GetAddress());
	D3D.GetDevContext()->PSSetConstantBuffers(0, 1, m_cb0.GetAddress());
	D3D.GetDevContext()->VSSetConstantBuffers(1, 1, m_cb1_Material.GetAddress());
	D3D.GetDevContext()->PSSetConstantBuffers(1, 1, m_cb1_Material.GetAddress());
}

//-----------------------------------------------------------------------------
// Name: DrawMesh()
// Desc: メッシュ描画
//-----------------------------------------------------------------------------
void KdModelShader::DrawMesh(const KdMesh* mesh, const std::vector<KdMaterial>& materials)
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

		// マテリアルセット
		const KdMaterial& material = materials[mesh->GetSubsets()[subi].MaterialNo];

		//-----------------------
		// マテリアル情報を定数バッファへ書き込む
		//-----------------------
		m_cb1_Material.Work().BaseColor = material.BaseColor;
		m_cb1_Material.Work().Emissive  = material.Emissive;
		m_cb1_Material.Work().Metallic  = material.Metallic;
		m_cb1_Material.Work().Roughness = material.Roughness;
		m_cb1_Material.Write();

		//-----------------------
		// テクスチャセット
		//-----------------------
		// BaseColor
		D3D.GetDevContext()->PSSetShaderResources(0, 1, material.BaseColorTex->GetSRViewAddress());
		// Emissive
		D3D.GetDevContext()->PSSetShaderResources(1, 1, material.EmissiveTex->GetSRViewAddress());
		// Metallic Roughness Map
		D3D.GetDevContext()->PSSetShaderResources(2, 1, material.MetallicRoughnessTex->GetSRViewAddress());
		// Normal Map
		D3D.GetDevContext()->PSSetShaderResources(3, 1, material.NormalTex->GetSRViewAddress());

		//-----------------------
		// サブセット描画
		//-----------------------
		mesh->DrawSubset(subi);
	}
}