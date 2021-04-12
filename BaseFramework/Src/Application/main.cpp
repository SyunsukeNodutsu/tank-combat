//-----------------------------------------------------------------------------
// File: main.cpp
//
// Edit: 2021/03/10 野筒隼輔
//-----------------------------------------------------------------------------
#include "main.h"
#include "imgui/ja_glyph_ranges.h"
#include "Game/Scene/Scene.h"
#include "Application/Manager/CameraManager.h"
#include "Application/Component/CameraComponent.h"

//-----------------------------------------------------------------------------
// Name: WinMain()
// Desc: メインエントリ
//-----------------------------------------------------------------------------
int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInst, LPSTR lpszArgs, int nWinMode)
{
	// メモリリークを知らせる
	_CrtSetDbgFlag(_CRTDBG_ALLOC_MEM_DF | _CRTDBG_LEAK_CHECK_DF);
	// COM初期化
	CoInitializeEx(nullptr, COINIT_MULTITHREADED);
	// mbstowcs_s関数で日本語対応
	setlocale(LC_ALL, "japanese");

	// 実行
	APP.Execute();

	// COM解放
	CoUninitialize();

	return 0;
}

//-----------------------------------------------------------------------------
// Name: Application()
// Desc: コンストラクタ
//-----------------------------------------------------------------------------
Application::Application()
	: m_window()
	, m_fps(0)
	, m_maxFps(60)
	, m_endFlag(false)
{
}

//-----------------------------------------------------------------------------
// Name: Init()
// Desc: 初期化
//-----------------------------------------------------------------------------
bool Application::Init(int width, int height)
{
	//--------------------------------------------------
	// ウィンドウ初期化
	//--------------------------------------------------

	// ウィンドウ作成
	if (m_window.Create(width, height, "tank combat 0.90", "Window") == false)
	{
		MessageBoxA(nullptr, "ウィンドウ作成に失敗", "エラー", MB_OK);
		return false;
	}

	// フルスクリーン確認
	bool isFullScreen = false;
	auto result = MessageBoxA(m_window.GetWndHandle(), "フルスクリーンにしますか？", "確認", MB_YESNO | MB_ICONQUESTION | MB_DEFBUTTON2);
	if (result == IDYES) { isFullScreen = true; }

	//--------------------------------------------------
	// Direct3D初期化
	//--------------------------------------------------

	// デバイスのデバッグモードを有効にする
	bool deviceDebugMode = false;
#ifdef _DEBUG
	deviceDebugMode = true;
#endif// _DEBUG

	// Direct3D初期化
	std::string errorMsg;
	if (D3D.Init(m_window.GetWndHandle(), width, height, deviceDebugMode, errorMsg) == false) {
		MessageBoxA(m_window.GetWndHandle(), errorMsg.c_str(), "Direct3D初期化失敗", MB_OK | MB_ICONSTOP);
		return false;
	}

	// フルスクリーン設定
	if (isFullScreen) { D3D.GetSwapChain()->SetFullscreenState(TRUE, 0); }

	// シェーダー初期化
	SHADER.Init();

	//--------------------------------------------------
	// オーディオ初期化
	//--------------------------------------------------
	// X2Audio.X3SAudio初期化
	AUDIO.Initialize();
	// 音量設定
	AUDIO.SetUserSettingVolume(1.0f);

	//--------------------------------------------------
	// ImGui初期化
	//--------------------------------------------------
	IMGUI_CHECKVERSION();
	ImGui::CreateContext();

	// ImGuiの外観設定
	ImGui::StyleColorsClassic();
//	ImGui::StyleColorsDark();

	ImGui::PushStyleColor(ImGuiCol_TitleBgActive, ImVec4(0.0f, 0.7f, 0.2f, 0.6f));
	ImGui::PushStyleColor(ImGuiCol_TitleBg, ImVec4(0.0f, 0.3f, 0.1f, 0.6f));

	ImGui_ImplWin32_Init(m_window.GetWndHandle());
	ImGui_ImplDX11_Init(D3D.GetDev(), D3D.GetDevContext());

	ImFontConfig config;
	config.MergeMode = true;

	ImGuiIO& io = ImGui::GetIO();
	io.Fonts->AddFontDefault();
	io.Fonts->AddFontFromFileTTF("c:\\Windows\\Fonts\\msgothic.ttc", 13.0f, &config, glyphRangesJapanese);

	//--------------------------------------------------
	// Effekseer初期化
	//--------------------------------------------------
	EFFEKSEER.Initialize();

	return true;
}

//-----------------------------------------------------------------------------
// Name: Release()
// Desc: 解放
//-----------------------------------------------------------------------------
void Application::Release()
{
	// ImGui
	ImGui_ImplDX11_Shutdown();
	ImGui_ImplWin32_Shutdown();
	ImGui::DestroyContext();

	// シェーダ解放
	SHADER.Release();
	// Direct3D解放
	D3D.Release();
	// オーディオ解放
	AUDIO.Release();
	// Effekseer解放
	EFFEKSEER.Finalize();

	// ウィンドウ削除
	m_window.Release();
}

//-----------------------------------------------------------------------------
// Name: Execute()
// Desc: 実行
//-----------------------------------------------------------------------------
void Application::Execute()
{
	//--------------------------------------------------
	// 初期設定(ウィンドウ作成、Direct3D初期化など)
	//--------------------------------------------------
	if (APP.Init(1280, 720) == false) { return; }

	// シーン生成
	SCENE.Deserialize();

	//--------------------------------------------------
	// ゲームループ
	//--------------------------------------------------

	// 時間
	DWORD baseTime = timeGetTime();
	int count = 0;

	// ループ
	while (1)
	{
		// 処理開始時間Get
		DWORD st = timeGetTime();

		// ゲーム終了指定があるときはループ終了
		if (m_endFlag) { break; }

		//--------------------------------------------------
		//
		// ウィンドウ関係の処理
		//
		//--------------------------------------------------

		// ウィンドウのメッセージを処理する
		m_window.ProcessMessage();

		// ウィンドウが破棄されてるならループ終了
		if (m_window.IsCreated() == false) { break; }

		//--------------------------------------------------
		//
		// オーディオ処理
		//
		//--------------------------------------------------
	
		// 使用中カメラ確認
		auto camera = CAMERA_MAGER.GetTargetCamera().lock();
		if (camera)
		{
			// listener設定
			Math::Vector3 listenerPos = camera->GetCameraMatrix().GetTranslation();
			Math::Vector3 listenerDir = camera->GetCameraMatrix().GetAxisZ();

			// 更新
			AUDIO.Update(listenerPos, listenerDir);
		}
		else
		{
			// カメラがない場合の更新
			AUDIO.Update(Math::Vector3(0.0f, 0.0f, 0.0f), Math::Vector3(0.0f, 0.0f, 1.0f));
		}

		//--------------------------------------------------
		//
		// ゲーム処理
		//
		//--------------------------------------------------

		// ImGui開始
		ImGui_ImplDX11_NewFrame();
		ImGui_ImplWin32_NewFrame();
		ImGui::NewFrame();

		// シーンのImGui更新
		SCENE.ImGuiUpdate();

		// バッファクリア
		D3D.GetDevContext()->ClearRenderTargetView(D3D.GetBackBuffer()->GetRTView(), Math::Color(0.0f, 1.0f, 1.0f, 1.0f));
		D3D.GetDevContext()->ClearDepthStencilView(D3D.GetZBuffer()->GetDSView(), D3D11_CLEAR_DEPTH | D3D11_CLEAR_STENCIL, 1.0f, 0);

		// シーンの更新
		SCENE.Update();
		// シーンの描画
		SCENE.Draw();

		// ImGui描画
		ImGui::Render();
		ImGui_ImplDX11_RenderDrawData(ImGui::GetDrawData());

		// バックバッファの画面表示
		D3D.GetSwapChain()->Present(0, 0);

		//--------------------------------------------------
		//
		// フレームレート制御
		//
		//--------------------------------------------------

		constexpr int	kSecond			 = 1000;// １秒のミリ秒
		constexpr float kFpsRefreshFrame =  500;// FPS計測のタイミング

		// 処理終了時間Get
		DWORD et = timeGetTime();
		// Fps制御
		DWORD ms = kSecond / m_maxFps;
		if (et - st < ms)
		{
			Sleep(ms - (et - st));	// 速すぎたら待つ
		}

		// FPS計測
		count++;
		if (st - baseTime >= kFpsRefreshFrame)
		{
			m_fps = (count * kSecond) / (st - baseTime);
			baseTime = st;
			count = 0;
		}
	}

	//--------------------------------------------------
	// アプリケーション解放
	//--------------------------------------------------
	Release();
}