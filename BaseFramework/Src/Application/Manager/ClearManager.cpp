//-----------------------------------------------------------------------------
// File: ClearManager.cpp
//
// Edit: 2021/03/27 野筒隼輔
//-----------------------------------------------------------------------------
#include "ClearManager.h"
#include "Application/Game/Other/Radio.h"
#include "Application/Game/Scene/Scene.h"
#include "Application/Game/Scene/Pause.h"
#include "Application/Game/Weapon/Tank.h"
#include "Application/Manager/CameraManager.h"

//-----------------------------------------------------------------------------
// Name: ClearManager()
// Desc: コンストラクタ
//-----------------------------------------------------------------------------
ClearManager::ClearManager()
	: m_friendlyTankList()
	, m_spClearStringTexture(nullptr)
	, m_spGameOverStringTexture(nullptr)
	, m_enemyCount(0)
	, m_textureColor(Math::Color(0.0f, 0.0f, 0.0f, 0.0f))
	, m_isClear(false)
	, m_isGameOver(false)
{
}

//-----------------------------------------------------------------------------
// Name: Deserialize()
// Desc: 初期化
//-----------------------------------------------------------------------------
void ClearManager::Deserialize(const json11::Json& json_object)
{
	LoadTexture();
	SettingParameter(json_object);
	m_isClear = m_isGameOver = false;
}

//-----------------------------------------------------------------------------
// Name: Update()
// Desc: 更新
//-----------------------------------------------------------------------------
void ClearManager::Update()
{
	if ((m_isClear == false) && (m_isGameOver == false)) { return; }
	if (!PAUSE.IsPlaying()) { return; }

	//--------------------------------------------------
	// クリア時の演出
	//--------------------------------------------------

	// 徐々に画像の明度を上昇
	static int cnt = 0;
	static float cnt2 = 0.0f;
	cnt++;
	if (cnt >= 60) {
		if (cnt2 < 1.0f) { cnt2 += 0.004f; }
		m_textureColor = Math::Color(cnt2, cnt2, cnt2, cnt2);
	}

	// 無線
	if (cnt == 200) {
		if (m_isClear) { RADIO.Emit(WIRELESS_TYPE::eReturn, OWNER_TYPE::eHQ, true, 180); }
	}

	// 遷移 -> Result
	if (cnt >= 480) {
		// 各値初期化
		m_textureColor = Math::Color(0.0f, 0.0f, 0.0f, 0.0f);
		cnt = 0;
		cnt2 = 0.0f;

		// 全エフェクト停止
		EFFEKSEER.StopAll();
		// 全サウンドフェードアウト
		AUDIO.AllFadeOut();
		// ゲームプロセスは終了
		PAUSE.SetGamePlaying(false);
		// 遷移->Result
		SCENE.RequestChangeScene("Data/Text/Process/ResultProcess.json");

		return;
	}
}

//-----------------------------------------------------------------------------
// Name: DrawSprite()
// Desc: 2D描画
//-----------------------------------------------------------------------------
void ClearManager::DrawSprite()
{
	if ((m_isClear == false) && (m_isGameOver == false)) { return; }
	if (!PAUSE.IsPlaying()) { return; }

	// テクスチャ設定 clear or feiled
	auto texture = (m_isClear) ? m_spClearStringTexture.get() : m_spGameOverStringTexture.get();

	// クリアの表示
	if (texture) {
		SHADER.m_spriteShader.SetMatrix(KdMatrix());
		SHADER.m_spriteShader.DrawTex(texture, 0, 0, nullptr, &m_textureColor);
	}
}

//-----------------------------------------------------------------------------
// Name: EnemyDefeat()
// Desc: 自軍が敵を撃破した際の通知
//-----------------------------------------------------------------------------
void ClearManager::EnemyDefeat()
{
	if (m_isGameOver) { return; }

	// 倒すべき敵のカウントを減少
	m_enemyCount--;

	// 敵の数が残り1 カメラアニメーションFlgON
	if (m_enemyCount == 1) {
		CAMERA_MAGER.SetCameraAnimation(true);
	}

	// 敵の数が残り0 ゲームクリア
	if (m_enemyCount <= 0) {
		m_isClear = true;
		CAMERA_MAGER.SetCameraAnimation(false);
	}
}

//-----------------------------------------------------------------------------
// Name: LoadTexture()
// Desc: テクスチャ読み込み
//-----------------------------------------------------------------------------
void ClearManager::LoadTexture()
{
	m_spClearStringTexture		= KdResFac.GetTexture("Data/Texture/ClearManager/Clear.png");
	m_spGameOverStringTexture	= KdResFac.GetTexture("Data/Texture/ClearManager/GameOver.png");
}

//-----------------------------------------------------------------------------
// Name: SettingParameter()
// Desc: jsonから変数の設定
//-----------------------------------------------------------------------------
void ClearManager::SettingParameter(const json11::Json& json_object)
{
	if (json_object.is_null()) { return; }

	// 撃破するべき敵の数
	if (!json_object["EnemyCount"].is_null()) { m_enemyCount = json_object["EnemyCount"].int_value(); }
}