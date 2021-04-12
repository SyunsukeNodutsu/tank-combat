//-----------------------------------------------------------------------------
// File: DebugManager.h
//
// Edit: 2021/03/29 野筒隼輔
//-----------------------------------------------------------------------------
#pragma once

//--------------------------------------------------
// 前方宣言
//--------------------------------------------------
class GameObject;

//-----------------------------------------------------------------------------
// Name: class DebugManager
// Desc: デバッグ管理 発表用のフラグなど
//-----------------------------------------------------------------------------
class DebugManager
{
public:

	// 初期化
	void Initialize();
	// jsonに保存
	void Serialize(const std::string a_path);
	// 更新
	void Update();
	// ImGui更新
	void ImGuiUpdate();

	//--------------------------------------------------
	// 所得
	//--------------------------------------------------
	// インスタンスの所得
	static DebugManager& GetInstance() { static DebugManager instance; return instance; }
	// LogWindow所得
	const std::shared_ptr<ImGuiLogWindow> GetLogWindow() const { return m_spLogWindow; }
	// ImGuiやマウスで選択したObject所得
	const std::weak_ptr<GameObject> GetSelectObject() const { return m_wpSelectObject; }

	//--------------------------------------------------
	// 所得 ※デバッグ.発表用
	//--------------------------------------------------
	// 無敵モードかどうか
	const bool IsNoDamage() const { return m_isNoDamage; }
	// コリジョン専用モデルの表示
	const bool IsCollModelShow() const { return m_isCollModelShow; }
	// OBBの表示
	const bool IsOBBShow() const { return m_isOBBShow; }

private:
	std::shared_ptr<ImGuiLogWindow>	m_spLogWindow;		// デバッグ用のLogを管理
	std::weak_ptr<GameObject>		m_wpSelectObject;	// ImGuiやマウスで選択したObject
	bool							m_isNoDamage;		// 無敵状態
	bool							m_isCollModelShow;	// コリジョンモデルの表示
	bool							m_isOBBShow;		// OBBの表示

private:
	DebugManager();
	void CreateObject();
	void GetJsonpath(const std::string& result);
	void SaveJson();
};

//-----------------------------------------------------------------------------
// Define: 簡単にアクセス
//-----------------------------------------------------------------------------
#define DEBUG_MAGER DebugManager::GetInstance()