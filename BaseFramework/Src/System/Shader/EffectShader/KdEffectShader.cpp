//-----------------------------------------------------------------------------
// File: KdEffectShader.cpp
//
// Edit: 初期配布
//-----------------------------------------------------------------------------
#include "System/KdSystem.h"
#include "KdEffectShader.h"

void KdEffectShader::DrawLine(const Math::Vector3& a_pos01, const Math::Vector3& a_pos02, const Math::Color& a_color)
{
	// 定数バッファ書き込み
	m_cb0.Write();

	// 頂点レイアウトをセット
	D3D.GetDevContext()->IASetInputLayout(m_inputLayout);

	// 頂点情報設定
	Vertex vertex[2] = {
		{ a_pos01, { 0, 0 }, a_color },
		{ a_pos02, { 1, 0 }, a_color },
	};

	// 頂点を描画
	D3D.DrawVertices(D3D_PRIMITIVE_TOPOLOGY_LINESTRIP, 2, &vertex[0], sizeof(Vertex));
}

void KdEffectShader::DrawVertices(const std::vector<Vertex>& a_vertices, D3D_PRIMITIVE_TOPOLOGY a_topology)
{
	// 定数バッファ書き込み
	m_cb0.Write();

	// 頂点レイアウトをセット
	D3D.GetDevContext()->IASetInputLayout(m_inputLayout);

	// 頂点を描画
	D3D.DrawVertices(a_topology, a_vertices.size(), &a_vertices[0], sizeof(Vertex));
}

void KdEffectShader::DrawMesh(const KdMesh* a_pMesh, const std::vector<KdMaterial>& a_materials)
{
	// メッシュ確認
	if (a_pMesh == nullptr) return;

	// 定数バッファ書き込み
	m_cb0.Write();

	// メッシュ情報をセット
	a_pMesh->SetToDevice();

	// 頂点レイアウトをセット
	D3D.GetDevContext()->IASetInputLayout(m_inputLayout_Model);

	// 全サブセット
	for (UINT subi = 0; subi < a_pMesh->GetSubsets().size(); subi++)
	{
		// 面が１枚も無い場合はスキップ
		if (a_pMesh->GetSubsets()[subi].FaceCount == 0)continue;

		// マテリアルセット
		const KdMaterial& material = a_materials[a_pMesh->GetSubsets()[subi].MaterialNo ];

		//-----------------------
		// テクスチャセット
		//-----------------------

		ID3D11ShaderResourceView* srvs[1] = {};
		// BaseColor
		srvs[0] = material.BaseColorTex->GetSRView();
		// セット
		D3D.GetDevContext()->PSSetShaderResources(0, _countof(srvs), srvs);

		//-----------------------
		// サブセット描画
		//-----------------------
		a_pMesh->DrawSubset(subi);
	}
}

bool KdEffectShader::Init()
{
	//-------------------------------------
	// 頂点シェーダ
	//-------------------------------------
	{
		// コンパイル済みのシェーダーヘッダーファイルをインクルード
		#include "KdEffectShader_VS.inc"

		// 頂点シェーダー作成
		if (FAILED(D3D.GetDev()->CreateVertexShader(compiledBuffer, sizeof(compiledBuffer), nullptr, &m_VS))) {
			assert(0 && "頂点シェーダー作成失敗");
			Release();
			return false;
		}

		{
			// １頂点の詳細な情報
			std::vector<D3D11_INPUT_ELEMENT_DESC> layout = {
				{ "POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT,		0,  0, D3D11_INPUT_PER_VERTEX_DATA, 0 },
				{ "TEXCOORD", 0, DXGI_FORMAT_R32G32_FLOAT,			0, 12, D3D11_INPUT_PER_VERTEX_DATA, 0 },
				{ "COLOR",	  0, DXGI_FORMAT_R32G32B32A32_FLOAT,	0, 20, D3D11_INPUT_PER_VERTEX_DATA, 0 },
			};

			// 頂点入力レイアウト作成
			if (FAILED(D3D.GetDev()->CreateInputLayout(
				&layout[0],				// 入力エレメント先頭アドレス
				layout.size(),			// 入力エレメント数
				&compiledBuffer[0],		// 頂点バッファのバイナリデータ
				sizeof(compiledBuffer),	// 上記のバッファサイズ
				&m_inputLayout))		// 
				)
			{
				assert(0 && "CreateInputLayout失敗");
				Release();
				return false;
			}
		}

		// DrawModel用
		{
			// １頂点の詳細な情報
			std::vector<D3D11_INPUT_ELEMENT_DESC> layout = {
				{ "POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT,		0,  0, D3D11_INPUT_PER_VERTEX_DATA, 0 },
				{ "TEXCOORD", 0, DXGI_FORMAT_R32G32_FLOAT,			0, 12, D3D11_INPUT_PER_VERTEX_DATA, 0 },
				{ "NORMAL",   0, DXGI_FORMAT_R32G32B32_FLOAT,		0, 20, D3D11_INPUT_PER_VERTEX_DATA, 0 },
				{ "COLOR",    0, DXGI_FORMAT_R8G8B8A8_UNORM,		0, 32, D3D11_INPUT_PER_VERTEX_DATA, 0 },
			};

			// 頂点入力レイアウト作成
			if (FAILED(D3D.GetDev()->CreateInputLayout(
				&layout[0],				// 入力エレメント先頭アドレス
				layout.size(),			// 入力エレメント数
				&compiledBuffer[0],		// 頂点バッファのバイナリデータ
				sizeof(compiledBuffer),	// 上記のバッファサイズ
				&m_inputLayout_Model))	// 
				)
			{
				assert(0 && "CreateInputLayout失敗");
				Release();
				return false;
			}		
		}
	}


	//-------------------------------------
	// ピクセルシェーダ
	//-------------------------------------
	{
		// コンパイル済みのシェーダーヘッダーファイルをインクルード
		#include "KdEffectShader_PS.inc"

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

	return true;
}

void KdEffectShader::Release()
{
	KdSafeRelease(m_VS);
	KdSafeRelease(m_PS);
	KdSafeRelease(m_inputLayout);
	KdSafeRelease(m_inputLayout_Model);
	m_cb0.Release();
}

void KdEffectShader::SetToDevice()
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
}

void KdEffectShader::SetTexture(ID3D11ShaderResourceView* a_pSrv)
{
	if (a_pSrv)
	{
		D3D.GetDevContext()->PSSetShaderResources(0, 1, &a_pSrv);
	}
	else
	{
		// テクスチャが無い場合は、白テクスチャをセットする
		D3D.GetDevContext()->PSSetShaderResources(0, 1, D3D.GetWhiteTex()->GetSRViewAddress());
	}
}