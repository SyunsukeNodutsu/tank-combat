//-----------------------------------------------------------------------------
// File: Scene.h
//
// Edit: 2021/04/02 野筒隼輔
//-----------------------------------------------------------------------------
#pragma once

//-----------------------------------------------------------------------------
// Name: class Scene
// Desc: シーン管理クラス ※修正.なんかシングルトンが多い気が
//-----------------------------------------------------------------------------
class Scene
{
public:

	// 初期化
	void Deserialize();
	// 解放
	void Release();
	// 更新
	void Update();
	// 描画
	void Draw();
	// ImGuiの更新
	void ImGuiUpdate();

	//--------------------------------------------------
	// 所得
	//--------------------------------------------------
	// インスタンスの所得
	static Scene& GetInstance() { static Scene instance; return instance; }
	// 現在のシーンのファイル名を所得
	const std::string GetNowSceneFilepath() const { return m_nowSceneFilepath; }
	// 編集カメラ使用
	const bool IsEdiorMode() const { return m_isEditorMode; }

	//--------------------------------------------------
	// 設定
	//--------------------------------------------------
	// カーソル表示のON.OFF切り替え ※修正.Sceneが管理すべきではない
	void SetCursorShow(const bool enable);
	// マウスカーソルをデスクトップの中央に
	const bool SetCenterCursor();
	// シーン変更のリクエストを受け付け
	void RequestChangeScene(const std::string& filepath);

private:
	std::string		m_nowSceneFilepath;		// 現在のシーンのJsonファイル名
	std::string		m_nextSceneFilepath;	// 次のシーンのJsonファイル名
	bool			m_isRequestChangeScene;	// シーン遷移のリクエストの有無
	bool			m_isEditorMode;			// 編集カメラ使用かどうか(ステージデバッグ)

private:
	Scene();
	void LoadScene(const std::string& filepath);
	void ExecChangeScene();
	void Reset();
	void StageSetting(const json11::Json json_object);
	void CheckModeChange();
};

//-----------------------------------------------------------------------------
// Define: 簡単にアクセス
//-----------------------------------------------------------------------------
#define SCENE Scene::GetInstance()