//-----------------------------------------------------------------------------
// File: ResultProcess.cpp
//
// Edit: 2021/03/27 野筒隼輔
//-----------------------------------------------------------------------------
#include "ResultProcess.h"
#include "Application/Game/Scene/Scene.h"
#include "Application/Game/Weapon/Tank.h"
#include "Application/Manager/ClearManager.h"
#include "Application/Manager/ObjectManager.h"

//-----------------------------------------------------------------------------
// Name: ResultProcess()
// Desc: コンストラクタ
//-----------------------------------------------------------------------------
ResultProcess::ResultProcess()
	: m_clearLank(CLEAR_LANK::eS)
	, m_spEasing(nullptr)
	, m_resultTexX(-940.0f)
	, m_textureAlpha(0.0f)
	, m_seCount(0)
	, m_isMissionComplete(false)

	, m_spBackTexture(nullptr)
	, m_spKeyTipsTexture(nullptr)
	, m_spResultOKTexture(nullptr)
	, m_spResultNOTexture(nullptr)
	, m_spAliveTexture(nullptr)
	, m_spDeadTexture(nullptr)
	, m_spNumberTextureArray()
	, m_spLankTextureArray()
{
}

//-----------------------------------------------------------------------------
// Name: Deserialize()
// Desc: 初期化
//-----------------------------------------------------------------------------
void ResultProcess::Deserialize(const json11::Json& json_object)
{
	GameObject::Deserialize(json_object);
	LoadTexture();
	SCENE.SetCursorShow(true);

	// イージング初期化
	m_spEasing = std::make_shared<Easing>();
	if (m_spEasing) { m_spEasing->Emit(EASING_TYPE::eBackOut, 0.6f, -940.0f, -330.0f); }

	// クリアしたか確認
	m_isMissionComplete = CLEAR_MAGER.IsGameClear();
	// クリアランク確認
	m_clearLank = (m_isMissionComplete) ? CheckClearLank() : CLEAR_LANK::eC;
}

//-----------------------------------------------------------------------------
// Name: Update()
// Desc: 更新
//-----------------------------------------------------------------------------
void ResultProcess::Update()
{
	// 遷移
	static bool isPush = true;
	if (GetAsyncKeyState(VK_LBUTTON) & 0x8000) {
		if (!isPush) {
			isPush = !isPush;
			AUDIO.Play("Data/Audio/SE/Enter.wav", 0.5f);
			SCENE.RequestChangeScene("Data/Text/Process/StageSelectProcess.json");
		}
	}
	else if (isPush) { isPush = !isPush; }

	// イージング更新
	UpdateEasing();

	// 指定フレームで効果音発生
	m_seCount++;
	if (m_seCount == 30) {
		// 効果音再生
		AUDIO.Play("Data/Audio/SE/Fanfare3.wav");
	}
}

//-----------------------------------------------------------------------------
// Name: DrawSprite()
// Desc: 2D描画
//-----------------------------------------------------------------------------
void ResultProcess::DrawSprite()
{
	// 背景
	if (m_spBackTexture) {
		SHADER.m_spriteShader.SetMatrix(KdMatrix());
		SHADER.m_spriteShader.DrawTex(m_spBackTexture.get(), 0, 0);
	}

	// キー入力のヒント
	if (m_spKeyTipsTexture) {
		SHADER.m_spriteShader.SetMatrix(KdMatrix());
		SHADER.m_spriteShader.DrawTex(m_spKeyTipsTexture.get(), 560, -330);
	}

	// 結果
	auto resultTex = (m_isMissionComplete) ? m_spResultOKTexture : m_spResultNOTexture;
	if (resultTex) {
		KdMatrix transMat = {};
		transMat.CreateTranslation(m_resultTexX, 302.0f, 0.0f);
		SHADER.m_spriteShader.SetMatrix(transMat);
		SHADER.m_spriteShader.DrawTex(resultTex.get(), 0, 0);
	}

	// クリアランク
	if (m_spLankTextureArray[m_clearLank]) {
		if (m_textureAlpha < 1.0f) { m_textureAlpha += 0.008f; }
		Math::Color color = { 1.0f, 1.0f, 1.0f, m_textureAlpha };
		SHADER.m_spriteShader.SetMatrix(KdMatrix());
		SHADER.m_spriteShader.DrawTex(m_spLankTextureArray[m_clearLank].get(), 355, -83, nullptr, &color);
	}

	// 戦果一覧
	DrawSpriteResult();
}

//-----------------------------------------------------------------------------
// Name: LoadTexture()
// Desc: テクスチャ読み込み
//-----------------------------------------------------------------------------
void ResultProcess::LoadTexture()
{
	m_spBackTexture		= KdResFac.GetTexture("Data/Texture/Result/Back.png");
	m_spKeyTipsTexture	= KdResFac.GetTexture("Data/Texture/Result/KeyTips.png");
	m_spResultOKTexture = KdResFac.GetTexture("Data/Texture/Result/Complete.png");
	m_spResultNOTexture = KdResFac.GetTexture("Data/Texture/Result/Failure.png");
	m_spAliveTexture	= KdResFac.GetTexture("Data/Texture/Result/Alive.png");
	m_spDeadTexture		= KdResFac.GetTexture("Data/Texture/Result/Dead.png");

	// クリアランク
	{
		m_spLankTextureArray.fill(nullptr);
		m_spLankTextureArray[CLEAR_LANK::eS] = KdResFac.GetTexture("Data/Texture/Result/ClearLank/S.png");
		m_spLankTextureArray[CLEAR_LANK::eA] = KdResFac.GetTexture("Data/Texture/Result/ClearLank/A.png");
		m_spLankTextureArray[CLEAR_LANK::eB] = KdResFac.GetTexture("Data/Texture/Result/ClearLank/B.png");
		m_spLankTextureArray[CLEAR_LANK::eC] = KdResFac.GetTexture("Data/Texture/Result/ClearLank/C.png");

		for (auto& texture : m_spLankTextureArray) {
			if (texture == nullptr) {
				assert(0 && "error load texture");
			}
		}
	}

	// 数字
	{
		m_spNumberTextureArray.fill(nullptr);
		m_spNumberTextureArray[0] = KdResFac.GetTexture("Data/Texture/Result/Number/0.png");
		m_spNumberTextureArray[1] = KdResFac.GetTexture("Data/Texture/Result/Number/1.png");
		m_spNumberTextureArray[2] = KdResFac.GetTexture("Data/Texture/Result/Number/2.png");
		m_spNumberTextureArray[3] = KdResFac.GetTexture("Data/Texture/Result/Number/3.png");
		m_spNumberTextureArray[4] = KdResFac.GetTexture("Data/Texture/Result/Number/4.png");
		m_spNumberTextureArray[5] = KdResFac.GetTexture("Data/Texture/Result/Number/5.png");
		m_spNumberTextureArray[6] = KdResFac.GetTexture("Data/Texture/Result/Number/6.png");
		m_spNumberTextureArray[7] = KdResFac.GetTexture("Data/Texture/Result/Number/7.png");
		m_spNumberTextureArray[8] = KdResFac.GetTexture("Data/Texture/Result/Number/8.png");
		m_spNumberTextureArray[9] = KdResFac.GetTexture("Data/Texture/Result/Number/9.png");

		for (auto& texture : m_spNumberTextureArray) {
			if (texture == nullptr) {
				assert(0 && "error load texture");
			}
		}
	}
}

//-----------------------------------------------------------------------------
// Name: UpdateEasing()
// Desc: Easing更新
//-----------------------------------------------------------------------------
void ResultProcess::UpdateEasing()
{
	if (m_spEasing == nullptr) { return; }
	if (!m_spEasing->IsEmit()) { return; }

	// 更新
	m_spEasing->Update();
	// 座標所得.設定
	m_resultTexX = m_spEasing->GetMovement();
}

//-----------------------------------------------------------------------------
// Name: DrawSpriteResult()
// Desc: 自軍の戦果 2D描画
//-----------------------------------------------------------------------------
void ResultProcess::DrawSpriteResult()
{
	uint8_t i = 0;
	for (auto& object : CLEAR_MAGER.GetFriendlyTankList()) {
		//--------------------------------------------------
		// 命中率
		//--------------------------------------------------
		uint8_t a = 0, b = 0, c = 0;
		std::tie(a, b, c) = CheckHitlate(object->GetAttackNum(), object->GetAttackHitNum());
		if (object->GetAttackNum() == 0) {
			// 攻撃回数が0である場合は0％
			SHADER.m_spriteShader.DrawTex(m_spNumberTextureArray[a].get(), -270 + 44, 86 - (i * 101));
		}
		else {
			if (m_spNumberTextureArray[a] && m_spNumberTextureArray[b] && m_spNumberTextureArray[c]) {
				SHADER.m_spriteShader.DrawTex(m_spNumberTextureArray[a].get(), -270 + 44, 86 - (i * 101));
				SHADER.m_spriteShader.DrawTex(m_spNumberTextureArray[b].get(), -270 + 22, 86 - (i * 101));
				if (c != 0) { SHADER.m_spriteShader.DrawTex(m_spNumberTextureArray[c].get(), -270, 86 - (i * 101)); }
			}
		}

		//--------------------------------------------------
		// 車番
		//--------------------------------------------------
		if (m_spNumberTextureArray[0] && m_spNumberTextureArray[i + 1]) {
			SHADER.m_spriteShader.DrawTex(m_spNumberTextureArray[0].get(), -598, 86 - (i * 101));
			SHADER.m_spriteShader.DrawTex(m_spNumberTextureArray[i + 1].get(), -598 + 24, 86 - (i * 101));
		}

		//--------------------------------------------------
		// 生存してるか
		//--------------------------------------------------
		auto srvTex = (object->IsAlive()) ? m_spAliveTexture : m_spDeadTexture;
		if (srvTex) {
			SHADER.m_spriteShader.SetMatrix(KdMatrix());
			SHADER.m_spriteShader.DrawTex(srvTex.get(), -392, 86 - (i * 101));
		}

		//--------------------------------------------------
		// 撃破数
		//--------------------------------------------------
		if (m_spNumberTextureArray[object->GetDefeatsNum()]) {
			SHADER.m_spriteShader.SetMatrix(KdMatrix());
			SHADER.m_spriteShader.DrawTex(m_spNumberTextureArray[object->GetDefeatsNum()].get(), -70, 86 - (i * 101));
		}

		// Spriteの配置用にカウント
		i++;
	}
}

//-----------------------------------------------------------------------------
// Name: CheckHitlate()
// Desc: 命中率の確認 ※100の位まで計算する
//-----------------------------------------------------------------------------
std::tuple<uint8_t, uint8_t, uint8_t> ResultProcess::CheckHitlate(uint8_t attakNum, uint8_t hitNum)
{
	float hitlate = static_cast<float>(hitNum) / static_cast<float>(attakNum);
	hitlate *= 100.0f;

	// 切り捨てしてUINTに変換
	hitlate = std::floorf(hitlate);
	uint8_t uiHitlate = static_cast<uint8_t>(hitlate);

	// 命中率(求めたい位で割る->10で割った余り...その桁の値)
	uint8_t _100thPlace		= (uiHitlate / 100) % 10;// 100の位
	uint8_t _10thPlace		= (uiHitlate / 10)	% 10;//  10の位
	uint8_t _1thPlace		= (uiHitlate / 1)	% 10;//   1の位

	// done
	return std::forward_as_tuple(_1thPlace, _10thPlace, _100thPlace);
}

//-----------------------------------------------------------------------------
// Name: CheckClearLank()
// Desc: クリアランクの確認
//-----------------------------------------------------------------------------
const CLEAR_LANK ResultProcess::CheckClearLank() const
{
	// チュートリアルだとSランク
	if (1) { return CLEAR_LANK::eS; }

	// スコアに応じてランク確定
	if (1) { return CLEAR_LANK::eS; }
	if (1) { return CLEAR_LANK::eA; }
	return CLEAR_LANK::eB;
}

//-----------------------------------------------------------------------------
// Name: SaveResult()
// Desc: 結果の保存
//-----------------------------------------------------------------------------
void ResultProcess::SaveResult(const std::string filepath)
{

}