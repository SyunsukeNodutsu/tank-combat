//-----------------------------------------------------------------------------
// File: TutorialProcess.cpp
//
// Edit: 2021/04/08 野筒隼輔
//-----------------------------------------------------------------------------
#include "TutorialProcess.h"

//-----------------------------------------------------------------------------
// Name: TutorialProcess()
// Desc: コンストラクタ
//-----------------------------------------------------------------------------
TutorialProcess::TutorialProcess()
	: m_spState(nullptr)
{
}

//-----------------------------------------------------------------------------
// Name: Deserialize()
// Desc: 初期化
//-----------------------------------------------------------------------------
void TutorialProcess::Deserialize(const json11::Json& json_object)
{
	GameProcess::Deserialize(json_object);

	// 状態初期化
	m_spState = std::make_shared<State01>();
	if (m_spState) { m_spState->Initialize(*this); }
}

//-----------------------------------------------------------------------------
// Name: Update()
// Desc: 更新
//-----------------------------------------------------------------------------
void TutorialProcess::Update()
{
	// 状態ごとの更新
	if (m_spState) { m_spState->Update(*this); }
}

//-----------------------------------------------------------------------------
// Name: DrawSprite()
// Desc: 2D描画
//-----------------------------------------------------------------------------
void TutorialProcess::DrawSprite()
{
	// 状態ごとの2D描画
	if (m_spState) { m_spState->DrawSprite(*this); }
}



//=============================================================================
// 
// State01
// 
//=============================================================================

//-----------------------------------------------------------------------------
// Name: State01()
// Desc: コンストラクタ
//-----------------------------------------------------------------------------
TutorialProcess::State01::State01()
	: m_progress(Progress::eStart)
	, m_spTextureArray()
{
}

//-----------------------------------------------------------------------------
// Name: Initialize()
// Desc: 初期化
//-----------------------------------------------------------------------------
void TutorialProcess::State01::Initialize(TutorialProcess& owner)
{
	// テクスチャ読み込み
	m_spTextureArray[0] = KdResFac.GetTexture("Data/Texture/Tutorial/Next.png");
	m_spTextureArray[1] = KdResFac.GetTexture("Data/Texture/Tutorial/01/01.png");
	m_spTextureArray[2] = KdResFac.GetTexture("Data/Texture/Tutorial/01/02.png");
	m_spTextureArray[3] = KdResFac.GetTexture("Data/Texture/Tutorial/01/03.png");
	m_spTextureArray[4] = KdResFac.GetTexture("Data/Texture/Tutorial/01/04.png");
	m_spTextureArray[5] = KdResFac.GetTexture("Data/Texture/Tutorial/01/05.png");
	m_spTextureArray[6] = KdResFac.GetTexture("Data/Texture/Tutorial/01/06.png");

	// 読み込み確認
	for (auto& texture : m_spTextureArray) {
		if (texture == nullptr) {
			assert(0 && "texture load error");
		}
	}
}

//-----------------------------------------------------------------------------
// Name: Update()
// Desc: 更新
//-----------------------------------------------------------------------------
void TutorialProcess::State01::Update(TutorialProcess& owner)
{
}

//-----------------------------------------------------------------------------
// Name: DrawSprite()
// Desc: 2D描画
//-----------------------------------------------------------------------------
void TutorialProcess::State01::DrawSprite(TutorialProcess& owner)
{
	switch (m_progress)
	{
	case Progress::e01:
		// の説明
		if (m_spTextureArray[0]) {
			SHADER.m_spriteShader.SetMatrix(KdMatrix());
			SHADER.m_spriteShader.DrawTex(m_spTextureArray[0].get(), 0, 0);
		}
		break;

	case Progress::e02:
		break;

	case Progress::e03:
		break;

	case Progress::e04:
		break;
	}
}



//=============================================================================
// 
// State02
// 
//=============================================================================

//-----------------------------------------------------------------------------
// Name: Initialize()
// Desc: 初期化
//-----------------------------------------------------------------------------
void TutorialProcess::State02::Initialize(TutorialProcess& owner)
{
}

//-----------------------------------------------------------------------------
// Name: Update()
// Desc: 更新
//-----------------------------------------------------------------------------
void TutorialProcess::State02::Update(TutorialProcess& owner)
{
}

//-----------------------------------------------------------------------------
// Name: DrawSprite()
// Desc: 2D描画
//-----------------------------------------------------------------------------
void TutorialProcess::State02::DrawSprite(TutorialProcess& owner)
{
}