//-----------------------------------------------------------------------------
// File: TutorialProcess.cpp
//
// Edit: 2021/04/13 野筒隼輔
//-----------------------------------------------------------------------------
#include "TutorialProcess.h"
#include "Application/Game/AI/Character/AutomaticTank.h"
#include "Application/Game/Scene/Pause.h"
#include "Application/Game/Scene/Scene.h"
#include "Application/Manager/CameraManager.h"
#include "Application/Manager/ClearManager.h"
#include "Application/Manager/ObjectManager.h"
#include "Application/Component/ModelComponent.h"

//-----------------------------------------------------------------------------
// Name: EaseIn()
// Desc: 矢印の上下運動用 簡易的なeasing
//-----------------------------------------------------------------------------
const float EaseIn(const float p)
{
	return 1.0f - cosf(p * static_cast<float>(M_PI)) * 0.5f;
}

//-----------------------------------------------------------------------------
// Name: TutorialProcess()
// Desc: コンストラクタ
//-----------------------------------------------------------------------------
TutorialProcess::TutorialProcess()
	: m_spState(nullptr)
	, m_startPos(KdVector3(0.0f, 0.0f, 0.0f))
	, m_goalPos(KdVector3(0.0f, 0.0f, 0.0f))
	, m_arrowProgress(0.0f)
	, m_goTo(false)
{
}

//-----------------------------------------------------------------------------
// Name: Deserialize()
// Desc: 初期化
//-----------------------------------------------------------------------------
void TutorialProcess::Deserialize(const json11::Json& json_object)
{
	GameProcess::Deserialize(json_object);
	SettingArrow(json_object);

	SCENE.SetCursorShow(false);
	PAUSE.SetGamePlaying(true);

	// クリア管理設定
	CLEAR_MAGER.Deserialize(KdResFac.GetJson(SCENE.GetNowSceneFilepath()));

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

	// 戦果用に味方戦車のコンテナをClearManagerに設定
	constexpr std::uint8_t count_max = 30;
	static std::uint8_t count = 0;
	count++;
	if (count >= count_max) {
		count = 0;
		// 味方戦車のコンテナを結果用に設定する
		std::list<std::shared_ptr<Tank>> friendlyTanklist = {};
		for (auto& tank : OBJ_MAGER.GetTankList()) {
			if (tank->GetTag() & OBJECT_TAG::TAG_Enemy) { continue; }
			friendlyTanklist.push_back(tank);
		}
		CLEAR_MAGER.SetFriendlyTankList(friendlyTanklist);
	}
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

//-----------------------------------------------------------------------------
// Name: UpdateArrow()
// Desc: 矢印更新 ※指定座標で上下運動
//-----------------------------------------------------------------------------
void TutorialProcess::UpdateArrow()
{
	if (m_spDrawModelComponent == nullptr) { return; }

	// 目標地点までのベクトル
	KdVector3 vTo = m_goalPos - m_startPos;
	// 進行具合を加味して現在の地点を算出
	KdVector3 vNow = m_startPos + vTo * EaseIn(m_arrowProgress);
	// 座標確定
	m_worldMatrix.SetTranslation(vNow);

	//--------------------------------------------------
	// 進行具合の更新
	//--------------------------------------------------

	// 速度
	constexpr float speed = 0.02f;

	// 向かっているとき
	if (m_goTo) {
		// 移動
		m_arrowProgress += speed;
		// 進行具合の更新
		if (m_arrowProgress >= 1.0f) {
			m_goTo = false;
			m_arrowProgress = 1.0f;
		}
	}
	// 返っているとき
	else {
		// 移動
		m_arrowProgress -= speed;
		// 進行具合の更新
		if (m_arrowProgress <= 0.0f) {
			m_goTo = true;
			m_arrowProgress = 0.0f;
		}
	}
}

//-----------------------------------------------------------------------------
// Name: SettingArrow()
// Desc: 矢印の初期設定
//-----------------------------------------------------------------------------
void TutorialProcess::SettingArrow(const json11::Json& json_object)
{
	// 開始座標
	m_startPos = m_worldMatrix.GetTranslation();
	// 終了座標
	const std::vector<json11::Json>& pos = json_object["GoTo"].array_items();
	if (pos.size() == 3) {
		m_goalPos.x = (static_cast<float>(pos[0].number_value()));
		m_goalPos.y = (static_cast<float>(pos[1].number_value()));
		m_goalPos.z = (static_cast<float>(pos[2].number_value()));
	}

	// 矢印は最初は見えない
	if (m_spDrawModelComponent) {
		m_spDrawModelComponent->SetEnable(false);
	}
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
	, m_isEnemyPop(false)
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

	// 敵が1両のみなので撃破時のカメラアニメーションON
	CAMERA_MAGER.SetCameraAnimation(true);
}

//-----------------------------------------------------------------------------
// Name: Update()
// Desc: 更新
//-----------------------------------------------------------------------------
void TutorialProcess::State01::Update(TutorialProcess& owner)
{
	static std::uint_fast8_t count = 0;
	switch (m_progress)
	{
		// すぐに始まらないように 数フレーム待つ
	case Progress::eStart:
		count++;
		if (count >= 30) {
			count = 0;
			m_progress = Progress::e01;
		}
		break;

		// 座標に誘導
	case Progress::e03:
		// 矢印更新
		owner.UpdateArrow();
		// 座標に到着したか確認
		if (CheckArrival(owner)) {
			owner.m_spDrawModelComponent->SetEnable(false);
			m_progress = Progress::e04;
		}
		break;

		// 敵を撃破
	case Progress::e04:
		if (!m_isEnemyPop) {
			m_isEnemyPop = !m_isEnemyPop;
			// Pop Enemy
			auto enemy = std::make_shared<AutomaticTank>();
			if (enemy == nullptr) { return; }
			auto json = KdResFac.GetJson("Data/Text/Object/Meta/AddEnemy_Tutorial_01.json");
			KdMergePrefab(json);
			enemy->Deserialize(json);
			enemy->Discovery();
			OBJ_MAGER.AddObject(enemy);
		}
		if (CLEAR_MAGER.IsGameClear()) { m_progress = Progress::eEnd; }
		break;
	}

	// SPACEキーで次へ
	static bool isPush = true;
	if (GetAsyncKeyState(VK_SPACE) & 0x8000) {
		if (!isPush) {
			isPush = !isPush;
			// 遷移
			switch (m_progress)
			{
			case Progress::e01: m_progress = Progress::e02; break;
			case Progress::e02: m_progress = Progress::e03; owner.m_spDrawModelComponent->SetEnable(true); break;
			}
		}
	}
	else if (isPush) { isPush = !isPush; }
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
		// 次へ
		if (m_spTextureArray[0]) {
			SHADER.m_spriteShader.SetMatrix(KdMatrix());
			SHADER.m_spriteShader.DrawTex(m_spTextureArray[0].get(), 0, -280);
		}
		// 車体情報UIの説明
		if (m_spTextureArray[1]) {
			SHADER.m_spriteShader.SetMatrix(KdMatrix());
			SHADER.m_spriteShader.DrawTex(m_spTextureArray[1].get(), -400, -80);
		}
		// 装填UIの説明
		if (m_spTextureArray[2]) {
			SHADER.m_spriteShader.SetMatrix(KdMatrix());
			SHADER.m_spriteShader.DrawTex(m_spTextureArray[2].get(), 400, -180);
		}
		break;

	case Progress::e02:
		// 次へ
		if (m_spTextureArray[0]) {
			SHADER.m_spriteShader.SetMatrix(KdMatrix());
			SHADER.m_spriteShader.DrawTex(m_spTextureArray[0].get(), 0, -280);
		}
		// カメラ操作の説明
		if (m_spTextureArray[3]) {
			SHADER.m_spriteShader.SetMatrix(KdMatrix());
			SHADER.m_spriteShader.DrawTex(m_spTextureArray[3].get(), 380, -160);
		}
		break;

	case Progress::e03:
		// 移動の説明
		if (m_spTextureArray[4]) {
			SHADER.m_spriteShader.SetMatrix(KdMatrix());
			SHADER.m_spriteShader.DrawTex(m_spTextureArray[4].get(), 380, -160);
		}
		break;

	case Progress::e04:
		// の説明
		if (m_spTextureArray[5]) {
			SHADER.m_spriteShader.SetMatrix(KdMatrix());
			SHADER.m_spriteShader.DrawTex(m_spTextureArray[5].get(), 380, -160);
		}
		// の説明
		if (m_spTextureArray[6]) {
			SHADER.m_spriteShader.SetMatrix(KdMatrix());
			SHADER.m_spriteShader.DrawTex(m_spTextureArray[6].get(), -380, -160);
		}
		break;
	}
}

//-----------------------------------------------------------------------------
// Name: CheckArrival()
// Desc: 目的地に到着したか
//-----------------------------------------------------------------------------
const bool TutorialProcess::State01::CheckArrival(TutorialProcess& owner) const
{
	// playerの検索
	std::shared_ptr<GameObject> player = nullptr;
	for (auto& tank : OBJ_MAGER.GetTankList()) {
		// TAG確認
		if (tank->GetTag() & OBJECT_TAG::TAG_Player) { player = tank; }
	}
	if (player == nullptr) {
		assert(0 && "player not found");
		return false;
	}

	// 距離判定
	KdVector3 playerPos = player->GetMatrix().GetTranslation();
	constexpr float tolerance = 4.0f;// 許容する値
	return (
		playerPos.x >= owner.m_startPos.x - tolerance && playerPos.x <= owner.m_startPos.x + tolerance &&
		playerPos.z >= owner.m_startPos.z - tolerance && playerPos.z <= owner.m_startPos.z + tolerance
		);
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