//-----------------------------------------------------------------------------
// File: KdUtility.cpp
//
// Edit: 2021/03/10 野筒隼輔
//-----------------------------------------------------------------------------
#include "KdUtility.h"
#include "Application/main.h"
#include "System/KdSystem.h"

//-----------------------------------------------------------------------------
// CommonTimer
//-----------------------------------------------------------------------------

void CommonTimer::SetTime(float a_sec)
{
	m_endTime = static_cast<int>(a_sec * APP.m_maxFps);
	Reset();
}

//-----------------------------------------------------------------------------
// RestoreRenderTarget
//-----------------------------------------------------------------------------

RestoreRenderTarget::RestoreRenderTarget()
{
	D3D.GetDevContext()->OMGetRenderTargets(1, &m_pSaveRT, &m_pSaveZ);
}

RestoreRenderTarget::~RestoreRenderTarget()
{
	// レンダーターゲット変更
	D3D.GetDevContext()->OMSetRenderTargets(1, &m_pSaveRT, m_pSaveZ);

	// 解放
	if (m_pSaveRT) m_pSaveRT->Release();
	if (m_pSaveZ)  m_pSaveZ->Release();
}

//-----------------------------------------------------------------------------
// その他
//-----------------------------------------------------------------------------

void KdGetTextureInfo(ID3D11View* apView, D3D11_TEXTURE2D_DESC& aOutDesc)
{
	aOutDesc = {};

	ID3D11Resource* res;
	apView->GetResource(&res);

	ID3D11Texture2D* tex2D;
	if (SUCCEEDED(res->QueryInterface<ID3D11Texture2D>(&tex2D)))
	{
		tex2D->GetDesc(&aOutDesc);
		tex2D->Release();
	}
	res->Release();
}

void KdMergePrefab(json11::Json& aSrcJson)
{
	// プレハブ指定ありの場合は、プレハブ側のものをベースにこのJSONをミックスする
	std::string prefabFilename = aSrcJson["Prefab"].string_value();
	if (prefabFilename == "None") return;
	if (prefabFilename.size() <= 0) return;

	// プレハブで指定したJsonの読み込み
	json11::Json prefJson = KdResFac.GetJson(prefabFilename);

	// 読み込み結果確認
	if (prefJson.is_null()) return;

	// JsonObject所得
	json11::Json::object copyPrefab = prefJson.object_items();

	// マージする
	for (auto&& jsonObj : aSrcJson.object_items())
	{
		copyPrefab[jsonObj.first] = jsonObj.second;
	}

	// マージしたものに差し替え
	aSrcJson = copyPrefab;
}