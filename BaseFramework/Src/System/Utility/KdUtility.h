//-----------------------------------------------------------------------------
// File: KdUtility.h
//
// Edit: 2021/03/10 野筒隼輔
//-----------------------------------------------------------------------------
#pragma once

//-----------------------------------------------------------------------------
//
// 便利機能
//
//-----------------------------------------------------------------------------

// 算術系短縮名
namespace Math = DirectX::SimpleMath;

// 角度変換
constexpr float KdToRadians = (3.141592654f / 180.0f);
constexpr float KdToDegrees = (180.0f / 3.141592654f);

// 安全にRelease
template<class T>
void KdSafeRelease(T*& p) { if (p) { p->Release(); p = nullptr; } }

// 安全にDelete
template<class T>
void KdSafeDelete(T*& p) { if (p) { delete p; p = nullptr; } }

//-----------------------------------------------------------------------------
// Name: struct CommonTimer
// Desc: 汎用タイマー
//-----------------------------------------------------------------------------
struct CommonTimer
{
	// 更新
	void Update()
	{
		m_count = std::min(++m_count, m_endTime);
	}

	// カウントリセット
	void Reset() { m_count = 0; }

	// 終了しているか
	bool IsEnd() { return (m_count >= m_endTime); }

	// 進行度の確認
	float GetProgress() { return static_cast<float>(m_count / m_endTime); }

	// 秒数(カウント)所得
	float GetCount() { return static_cast<float>(m_count); }

	// 秒単位で設定
	void SetTime(float a_sec);

private:
	int m_count = 0;	// タイマーカウント
	int m_endTime = 0;	// タイマーの終点
};

//-----------------------------------------------------------------------------
// Name: class RestoreRenderTarget
// Desc: レンダーターゲットを自動で元に戻す便利クラス
//-----------------------------------------------------------------------------
class RestoreRenderTarget
{
public:
	RestoreRenderTarget();
	~RestoreRenderTarget();

private:
	ID3D11RenderTargetView* m_pSaveRT = nullptr;
	ID3D11DepthStencilView* m_pSaveZ  = nullptr;
};

//-----------------------------------------------------------------------------
//
// 色定数
//
//-----------------------------------------------------------------------------
static const Math::Color	kWhiteColor		= Math::Color(1, 1, 1, 1);
static const Math::Color	kBlackColor		= Math::Color(0, 0, 0, 1);
static const Math::Color	kRedColor		= Math::Color(1, 0, 0, 1);
static const Math::Color	kGreenColor		= Math::Color(0, 1, 0, 1);
static const Math::Color	kBlueColor		= Math::Color(0, 0, 1, 1);

static const Math::Color	kTranslucent	= Math::Color(1, 1, 1, 0.6f);
static const Math::Color	kTranslucent2	= Math::Color(1, 1, 1, 0.4f);
static const Math::Color	kTranslucent3	= Math::Color(1, 1, 1, 0.8f);


//-----------------------------------------------------------------------------
//
// ファイル
//
//-----------------------------------------------------------------------------

// ファイルパスから、親ディレクトリまでのパスを取得
inline std::string KdGetDirFromPath(const std::string &a_path)
{
	const std::string::size_type pos = std::max<signed>(a_path.find_last_of('/'), a_path.find_last_of('\\'));
	return (pos == std::string::npos) ? std::string() : a_path.substr(0, pos + 1);
}

//-----------------------------------------------------------------------------
//
// 文字列関係
//
//-----------------------------------------------------------------------------

// std::string版 sprintf
template <typename ... Args>
std::string KdFormat(const std::string& a_fmt, Args ... a_args)
{
	size_t len = std::snprintf(nullptr, 0, a_fmt.c_str(), a_args ...);
	std::vector<char> buf(len + 1);
	std::snprintf(&buf[0], len + 1, a_fmt.c_str(), a_args ...);
	return std::string(&buf[0], &buf[0] + len);
}

//-----------------------------------------------------------------------------
//
// テクスチャ
//
//-----------------------------------------------------------------------------

//--------------------------------------------------
// Name: KdGetTextureInfo()
// Desc: viewから画像情報を所得
//--------------------------------------------------
void KdGetTextureInfo(ID3D11View* apView, D3D11_TEXTURE2D_DESC& aOutDesc);


//-----------------------------------------------------------------------------
//
// JSON
//
//-----------------------------------------------------------------------------

//--------------------------------------------------
// Name: KdMergePrefab()
// Desc: プレハブのマージ
//--------------------------------------------------
void KdMergePrefab(json11::Json& aSrcJson);


//-----------------------------------------------------------------------------
//
// ImGui
//
//-----------------------------------------------------------------------------

// --------------------------------------------------
// Name: class ImGuiLogWindow
// Desc: ImGuiのログ表示を管理
//--------------------------------------------------
class ImGuiLogWindow
{
public:
	// ログをクリア
	void Clear() { m_Buf.clear(); }

	// 文字列を追加
	template<class... Args>
	void AddLog(const char* a_fmt, Args... a_args)
	{
		// 改行を加える
		std::string str = a_fmt;
		str += "\n";
		m_Buf.appendf(str.c_str(), a_args...);
		m_ScrollToBottom = true;
		cnt = 120;
	}

	// ウィンドウ描画
	void ImGuiUpdate(const char* a_title, bool* a_pOpened = NULL)
	{
		if (ImGui::Begin(a_title, a_pOpened))//, ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoTitleBar
		{
			ImGui::TextUnformatted(m_Buf.begin());
			if (m_ScrollToBottom) ImGui::SetScrollHere(1.0f);
			m_ScrollToBottom = false;
		}
		ImGui::End();

		if (cnt > 0)
		{
			cnt--;
			if (cnt <= 0) Clear();
		}
	}

private:
	ImGuiTextBuffer m_Buf = {};
	bool			m_ScrollToBottom = false;

	int cnt = 0;
};