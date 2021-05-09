//-----------------------------------------------------------------------------
// File: Scene.cpp
//
// Edit: 2021/04/02 野筒隼輔
//-----------------------------------------------------------------------------
#include "Scene.h"
#include "Pause.h"
#include "InterruptCheck.h"
#include "Application/main.h"
#include "Application/Game/AI/Navigation/WayPointNavigator.h"
#include "Application/Game/Other/Radio.h"

#include "Application/Manager/CameraManager.h"
#include "Application/Manager/ClearManager.h"
#include "Application/Manager/DebugManager.h"
#include "Application/Manager/DrawManager.h"
#include "Application/Manager/ObjectManager.h"

#include "Application/Component/CameraComponent.h"
#include "Application/Component/ModelComponent.h"

//-----------------------------------------------------------------------------
// Name: Scene()
// Desc: コンストラクタ
//-----------------------------------------------------------------------------
Scene::Scene()
	: m_nowSceneFilepath("")
	, m_nextSceneFilepath("")
	, m_isRequestChangeScene(false)
	, m_isEditorMode(false)
{
}

//-----------------------------------------------------------------------------
// Name: Deserialize()
// Desc: 初期化
//-----------------------------------------------------------------------------
void Scene::Deserialize()
{
	// 描画管理初期化
	DRAW_MAGER.Initialize();
	// デバッグ管理初期化
	DEBUG_MAGER.Initialize();
	// ポーズ管理初期化
	PAUSE.Deserialize();
	// 無線初期化
	RADIO.Deserialize();
	// 割り込み確認管理初期化
	IRQ.Deserialize();

	// 音量設定
	AUDIO.SetUserSettingVolume(0.2f);

	//--------------------------------------------------
	// 開始シーン
	//--------------------------------------------------
	LoadScene("Data/Text/Process/TitleAnimationProcess_01.json");
}

//-----------------------------------------------------------------------------
// Name: Release()
// Desc: 解放
//-----------------------------------------------------------------------------
void Scene::Release()
{
	OBJ_MAGER.Reset();
}

//-----------------------------------------------------------------------------
// Name: Update()
// Desc: 更新
//-----------------------------------------------------------------------------
void Scene::Update()
{
	// 遷移の予約確認...予約があれば遷移
	if (m_isRequestChangeScene) { ExecChangeScene(); }

	// デバッグ更新
	DEBUG_MAGER.Update();
	// クリア管理更新
	CLEAR_MAGER.Update();
	// カメラ管理更新
	CAMERA_MAGER.Update();
	// 無線更新
	RADIO.Update();
	// ナビゲーション管理更新
	WAYP_NAVI.Update();
	WAYP_NAVI.DrawDebug();
	// 一時停止管理更新
	PAUSE.Update();
	// 割り込み管理更新
	IRQ.Update();

	// 一時停止中か確認
	if (!PAUSE.IsEnable()) {
		// 編集モード切替確認
		CheckModeChange();
		// 登場するObjectたちの更新
		OBJ_MAGER.Update();
	}
}

//-----------------------------------------------------------------------------
// Name: Draw()
// Desc: 描画
//-----------------------------------------------------------------------------
void Scene::Draw()
{
	// カメラ設定に応じてシェーダに設定
	CAMERA_MAGER.SetToShader();
	// 登場するObject達を描画
	DRAW_MAGER.Draw();
}

//-----------------------------------------------------------------------------
// Name: ImGuiUpdate()
// Desc: ImGui更新
//-----------------------------------------------------------------------------
void Scene::ImGuiUpdate()
{
	//--------------------------------------------------
	// シーンウィンドウ
	//--------------------------------------------------
	if (ImGui::Begin("Scene", false, ImGuiWindowFlags_NoMove)) {
		// シーンの情報
		ImGui::Text(m_nowSceneFilepath.c_str());
		ImGui::Text("Time      : %d", static_cast<UINT>(timeGetTime()));
		ImGui::Text("FPS       : %d", APP.m_fps);
		ImGui::Text("ThreadNum : %d", std::thread::hardware_concurrency());
		ImGui::Text("ThreadID  : %d", std::this_thread::get_id());

		if (APP.m_window.GetWinInfo().dwWindowStatus & WS_ACTIVECAPTION) { ImGui::Text("State     : ACTIVE"); }
		else { ImGui::Text("State     : NOT ACTIVE"); }

		// デバッグ.発表用
		DEBUG_MAGER.ImGuiUpdate();
	}
	ImGui::End();

	//--------------------------------------------------
	// ログウィンドウ
	//--------------------------------------------------
	DEBUG_MAGER.GetLogWindow()->ImGuiUpdate("Log");
}

//-----------------------------------------------------------------------------
// Name: SetCursorShow()
// Desc: マウスカーソルの表示ON.OFF切り替え
//-----------------------------------------------------------------------------
void Scene::SetCursorShow(const bool enable)
{
	if (enable) { while (ShowCursor(TRUE) < 0) {} }
	else { while (ShowCursor(FALSE) >= 0) {} }
}

//-----------------------------------------------------------------------------
// Name: SetCenterCursor()
// Desc: マウスカーソルをデスクトップの中央に
//-----------------------------------------------------------------------------
const bool Scene::SetCenterCursor()
{
	// 停止させる座標
	RECT rect = {};
	SystemParametersInfo(SPI_GETWORKAREA, 0, &rect, 0);// デスクトップの情報所得
	const int stopX = (rect.right - rect.left) / 2;
	const int stopY = (rect.bottom - rect.top) / 2;
	// 画面外に出ないように固定
	return SetCursorPos(stopX, stopY);
}

//-----------------------------------------------------------------------------
// Name: RequestChangeScene()
// Desc: シーン遷移の予約
//-----------------------------------------------------------------------------
void Scene::RequestChangeScene(const std::string& filepath)
{
	m_nextSceneFilepath = filepath;
	m_isRequestChangeScene = true;
}

//-----------------------------------------------------------------------------
// Name: LoadScene()
// Desc: シーン読み込み ※修正.json一括読み込みが遅さの一番の要因.特に文字列
//-----------------------------------------------------------------------------
void Scene::LoadScene(const std::string& filepath)
{
	// 各項目のクリア
	Reset();

	// 現在のシーンのファイル名保存
	m_nowSceneFilepath = filepath;

	// Json読み込み
	json11::Json json_object = KdResFac.GetJson(filepath);
	if (json_object.is_null()) {
		assert(0 && u8"[LoadScene] : jsonファイル読み込み失敗");
		return;
	}

	//--------------------------------------------------
	// ステージごとのライトやフォグの設定
	//--------------------------------------------------
	StageSetting(json_object);
	// ナビゲーション管理
	WAYP_NAVI.Initialize();

	//--------------------------------------------------
	// オブジェクト生成ループ
	//--------------------------------------------------

	auto& objectDataList = json_object["GameObjectList"].array_items();

	for (auto&& objJsonData : objectDataList) {
		// 生成
		auto new_object = CreateGameObject(objJsonData["ClassName"].string_value());
		if (new_object == nullptr) { continue; }
		// 初期化
		KdMergePrefab(objJsonData);
		new_object->Deserialize(objJsonData);
		// Objectコンテナに格納
		OBJ_MAGER.AddObject(new_object);
	}
}

//-----------------------------------------------------------------------------
// Name: ExecChangeScene()
// Desc: シーン遷移実行
//-----------------------------------------------------------------------------
void Scene::ExecChangeScene()
{
	LoadScene(m_nextSceneFilepath);
	m_isRequestChangeScene = false;
}

//-----------------------------------------------------------------------------
// Name: Reset()
// Desc: シーン遷移時のリセット
//-----------------------------------------------------------------------------
void Scene::Reset()
{
	// 使用カメラ
	CAMERA_MAGER.Reset();
	// その他
	EFFEKSEER.StopAll();
	OBJ_MAGER.Reset();
}

//-----------------------------------------------------------------------------
// Name: StageSetting()
// Desc: シーン遷移時の設定 ※修正 shaderに書いたほうがいいかも
//-----------------------------------------------------------------------------
void Scene::StageSetting(const json11::Json json_object)
{
	// フォグ減衰率
	if (!json_object["DistanceFogDensity"].is_null()) {
		SHADER.m_cb7_Camera.Work().DistanceFogDensity = static_cast<float>(json_object["DistanceFogDensity"].number_value());
	}

	// ライト色
	if (!json_object["DL_Color"].is_null()) {
		const std::vector<json11::Json>& DL_Color = json_object["DL_Color"].array_items();
		SHADER.m_cb8_Light.Work().DL_Color = {
			static_cast<float>(DL_Color[0].number_value()),
			static_cast<float>(DL_Color[1].number_value()),
			static_cast<float>(DL_Color[2].number_value())
		};
	}

	// フォグ色
	if (!json_object["DistanceFogColor"].is_null()) {
		const std::vector<json11::Json>& DistanceFogColor = json_object["DistanceFogColor"].array_items();
		SHADER.m_cb7_Camera.Work().DistanceFogColor = {
			static_cast<float>(DistanceFogColor[0].number_value()),
			static_cast<float>(DistanceFogColor[1].number_value()),
			static_cast<float>(DistanceFogColor[2].number_value())
		};
	}
}

//-----------------------------------------------------------------------------
// Name: CheckModeChange()
// Desc: 編集モード切替
//-----------------------------------------------------------------------------
void Scene::CheckModeChange()
{
	if (!PAUSE.IsPlaying()) { return; }// ゲーム中のみ

	static bool isPush = true;
	if (GetAsyncKeyState('1') & 0x8000) {
		if (!isPush) {
			isPush = !isPush;
			m_isEditorMode = !m_isEditorMode;
			SetCursorShow(m_isEditorMode);

			// トグルカメラ
			if (m_isEditorMode) {
				auto editorCamera = OBJ_MAGER.SearchObject("EditorCamera");
				if (editorCamera == nullptr) { return; }
				auto camera = editorCamera->GetCameraComponent();
				if (camera == nullptr) { return; }
				CAMERA_MAGER.SetTargetCamera(camera);
			}
			else {
				auto playerCamera = OBJ_MAGER.SearchObject("M4_Player");// 修正.Playerの名前を定数に
				if (playerCamera == nullptr) { return; }
				auto camera = playerCamera->GetCameraComponent();
				if (camera == nullptr) { return; }
				CAMERA_MAGER.SetTargetCamera(camera);
			}
		}
	}
	else if (isPush) { isPush = !isPush; }
}