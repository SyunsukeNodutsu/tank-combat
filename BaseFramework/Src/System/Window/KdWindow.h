﻿//-----------------------------------------------------------------------------
// File: KdWindow.h
//
// Edit: 2021/03/22 野筒隼輔
//-----------------------------------------------------------------------------
#pragma once

//-----------------------------------------------------------------------------
// Name: class KdWindow
// Desc: ウィンドウクラス
//-----------------------------------------------------------------------------
class KdWindow
{
public:

	// コンストラクタ
	KdWindow();
	// デストラクタ
	~KdWindow() { Release(); }
	// ウィンドウ作成
	bool Create(int clientWidth, int clientHeight, const std::string& titleName, const std::string& windowClassName);
	// 解放
	void Release();
	// ウィンドウメッセージを処理する ※終了メッセージが来たらfalseが返る
	bool ProcessMessage();
	// ファイルを開くダイアログボックスを表示
	static bool OpenFileDialog(std::string& filepath, const std::string& title = "ファイルを開く", const char* filters = "全てのファイル\0*.*\0");
	// ファイル名をつけて保存ダイアログボックスを表示
	static bool SaveFileDialog(std::string& filepath, const std::string& title = "ファイルを保存", const char* filters = "全てのファイル\0*.*\0", const std::string& defExt = "");

	//--------------------------------------------------
	// 取得・設定
	//--------------------------------------------------
	// ウィンドウハンドル取得
	HWND GetWndHandle() const { return m_hWnd; }
	// ウィンドウ情報所得
	WINDOWINFO GetWinInfo() const { return m_windowInfo; }
	// ウィンドウが存在するかどうか
	bool IsCreated() const { return m_hWnd ? true : false; }
	// マウスホイールの変化量を取得
	int GetMouseWheelVal() const { return m_mouseWheelVal; }
	// インスタンスハンドル取得
	HINSTANCE GetInstanceHandle() const { return GetModuleHandle(0); }
	// クライアントサイズの設定
	void SetClientSize(int width, int height);

private:
	HWND		m_hWnd;				// ウィンドウハンドル
	WINDOWINFO	m_windowInfo;		// ウィンドウ情報
	int			m_mouseWheelVal;	// マウスホイール値

private:
	static LRESULT CALLBACK callWindowProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam);
	LRESULT CALLBACK WindowProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam);
	BOOL SetDesktopCenterWindow(HWND hWnd);
};

/*
■ドキュメント
・OpenFileDialog
filepath	… 選択されたファイルパスが入る
title		… ウィンドウのタイトル文字
filters		… 指定された拡張子のみ表示されるようになる

・SaveFileDialog
filepath	… 選択されたファイルパスが入る
title		… ウィンドウのタイトル文字
filters		… 指定された拡張子のみ表示されるようになる
defExt		… ユーザーが拡張子を入力しなかった場合、これが自動的に付く
*/