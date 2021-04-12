//-----------------------------------------------------------------------------
// File: Radio.cpp
//
// Edit: 2021/03/27 野筒隼輔
//-----------------------------------------------------------------------------
#include "Radio.h"

//-----------------------------------------------------------------------------
// Name: Radio()
// Desc: コンストラクタ
//-----------------------------------------------------------------------------
Radio::Radio()
	: m_stringType()
	, m_ownerType()
	, m_enableCount(0)
{
}

//-----------------------------------------------------------------------------
// Name: Deserialize()
// Desc: 初期化
//-----------------------------------------------------------------------------
void Radio::Deserialize()
{
	LoadTexture();
}

//-----------------------------------------------------------------------------
// Name: Update()
// Desc: 更新
//-----------------------------------------------------------------------------
void Radio::Update()
{
	// 描画カウントの消費
	if (m_enableCount >= 0) { m_enableCount--; }
}

//-----------------------------------------------------------------------------
// Name: DrawSprite()
// Desc: 2D描画
//-----------------------------------------------------------------------------
void Radio::DrawSprite()
{
	// カウントが0以下だと描画しない
	if (m_enableCount <= 0) { return; }

	//--------------------------------------------------
	// 描画
	//--------------------------------------------------

	// 無線用ウィンドウ
	if (m_spRectTexture) {
		SHADER.m_spriteShader.SetMatrix(KdMatrix());
		SHADER.m_spriteShader.DrawTex(m_spRectTexture.get(), 0, -280, nullptr, &kTranslucent3);
	}

	// オーナー
	if (m_spOwnerTextureMap[m_ownerType]) {
		SHADER.m_spriteShader.SetMatrix(KdMatrix());
		SHADER.m_spriteShader.DrawTex(m_spOwnerTextureMap[m_ownerType].get(), 0, -280);
	}

	// ウィンドウ内に文字列
	if (m_spStringTextureMap[m_stringType]) {
		SHADER.m_spriteShader.SetMatrix(KdMatrix());
		SHADER.m_spriteShader.DrawTex(m_spStringTextureMap[m_stringType].get(), 0, -280);
	}
}

//-----------------------------------------------------------------------------
// Name: Emit()
// Desc: 発生
//-----------------------------------------------------------------------------
void Radio::Emit(WIRELESS_TYPE stringType, OWNER_TYPE ownerType, const bool sePlay, const int count)
{
	m_stringType = stringType;
	m_ownerType = ownerType;
	m_enableCount = count;
	if (sePlay) { AUDIO.Play("Data/Audio/SE/Radio.wav"); }
}

//-----------------------------------------------------------------------------
// Name: LoadTexture()
// Desc: テクスチャの読み込み
//-----------------------------------------------------------------------------
void Radio::LoadTexture()
{
	// 無線ウィンドウの矩形
	m_spRectTexture = KdResFac.GetTexture("Data/Texture/Radio/Window.png");

	// 無線の内容(文字列)
	m_spStringTextureMap[WIRELESS_TYPE::eStartAttack]		= KdResFac.GetTexture("Data/Texture/Radio/String/StartAttack.png");
	m_spStringTextureMap[WIRELESS_TYPE::eDefeat]			= KdResFac.GetTexture("Data/Texture/Radio/String/Defeat.png");
	m_spStringTextureMap[WIRELESS_TYPE::eBeShot]			= KdResFac.GetTexture("Data/Texture/Radio/String/BeShot.png");
	m_spStringTextureMap[WIRELESS_TYPE::eRepelled]			= KdResFac.GetTexture("Data/Texture/Radio/String/Repelled.png");
	m_spStringTextureMap[WIRELESS_TYPE::eDiscovery]			= KdResFac.GetTexture("Data/Texture/Radio/String/Discovery.png");
	m_spStringTextureMap[WIRELESS_TYPE::eReturn]			= KdResFac.GetTexture("Data/Texture/Radio/String/StageClear.png");
	m_spStringTextureMap[WIRELESS_TYPE::eDeathAllies]		= KdResFac.GetTexture("Data/Texture/Radio/String/DeathAllies.png");
	m_spStringTextureMap[WIRELESS_TYPE::eFriendlyFire]		= KdResFac.GetTexture("Data/Texture/Radio/String/FriendlyFire.png");
	m_spStringTextureMap[WIRELESS_TYPE::eAdvanceOrder]		= KdResFac.GetTexture("Data/Texture/Radio/String/AdvanceOrder.png");
	m_spStringTextureMap[WIRELESS_TYPE::eRecessionOrder]	= KdResFac.GetTexture("Data/Texture/Radio/String/RecessionOrder.png");
	m_spStringTextureMap[WIRELESS_TYPE::eFollowOrder]		= KdResFac.GetTexture("Data/Texture/Radio/String/FollowOrder.png");
	m_spStringTextureMap[WIRELESS_TYPE::eWaitOrder]			= KdResFac.GetTexture("Data/Texture/Radio/String/WaitOrder.png");

	// オーナー
	m_spOwnerTextureMap[OWNER_TYPE::ePlayer]	= KdResFac.GetTexture("Data/Texture/Radio/Owner/OwnerPlayer.png");
	m_spOwnerTextureMap[OWNER_TYPE::eAllies]	= KdResFac.GetTexture("Data/Texture/Radio/Owner/OwnerAllies02.png");
	m_spOwnerTextureMap[OWNER_TYPE::eAllies02]	= KdResFac.GetTexture("Data/Texture/Radio/Owner/OwnerAllies02.png");
	m_spOwnerTextureMap[OWNER_TYPE::eAllies03]	= KdResFac.GetTexture("Data/Texture/Radio/Owner/OwnerAllies03.png");
	m_spOwnerTextureMap[OWNER_TYPE::eHQ]		= KdResFac.GetTexture("Data/Texture/Radio/Owner/OwnerHQ.png");

	// 読み込み結果の確認
	for (auto& texture : m_spStringTextureMap) {
		if (texture.second == nullptr) {
			assert(0 && "[LoadTexture()] : error loadtexture");
			return;
		}
	}
	for (auto& texture : m_spOwnerTextureMap) {
		if (texture.second == nullptr) {
			assert(0 && "[LoadTexture()] : error loadtexture");
			return;
		}
	}
}