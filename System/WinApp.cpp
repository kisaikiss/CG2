#include "WinApp.h"


//ウィンドウに発生する様々なイベントを処理するための関数
//ウィンドウプロシージャ
LRESULT CALLBACK WindowProc(HWND hwnd, UINT msg, WPARAM wpram, LPARAM lparam) {
	//ImGuiにメッセージを渡す。ImGuiが処理中なら後続の処理を打ち切る
	if (ImGui_ImplWin32_WndProcHandler(hwnd, msg, wpram, lparam)) {
		return true;
	}
	//メッセージに応じてゲーム固有の処理を行う
	switch (msg) {
		//ウィンドウが破棄された
	case WM_DESTROY:
		//OSに対して、アプリの終了を伝える
		PostQuitMessage(0);
		return 0;
	}
	//標準のメッセージ処理を行う
	return DefWindowProc(hwnd, msg, wpram, lparam);
}

bool WinApp::Create(const std::wstring& titleName, const std::wstring& windowClassName, int32_t clientWidth, int32_t clientHeight) {
	//ウィンドウプロシージャ
	wndClass_.lpfnWndProc = WindowProc;
	//ウィンドウクラス名
	wndClass_.lpszClassName = windowClassName.c_str();
	//インスタンスハンドル
	wndClass_.hInstance = GetModuleHandle(nullptr);
	//カーソル
	wndClass_.hCursor = LoadCursor(nullptr, IDC_ARROW);

	//ウィンドウクラスを登録する
	if (!RegisterClass(&wndClass_)) {
		return false;
	}

	//ウィンドウサイズを表す構造体にクライアント領域を入れる
	RECT wrc = { 0,0,clientWidth,clientHeight };

	//クライアント領域を元に実際のサイズにwrcを変更してもらう
	AdjustWindowRect(&wrc, WS_OVERLAPPEDWINDOW, false);

	//ウィンドウの生成
	hWnd_ = CreateWindow(
		wndClass_.lpszClassName,		//利用するクラス名
		titleName.c_str(),		//タイトルバーの文字
		WS_OVERLAPPEDWINDOW,	//良く見るウィンドウスタイル
		CW_USEDEFAULT,			//表示X座標(Windowsに任せる)
		CW_USEDEFAULT,			//表示Y座標(Windowsに任せる)
		wrc.right - wrc.left,	//ウィンドウ横幅
		wrc.bottom - wrc.top,	//ウィンドウ縦幅
		nullptr,				//親ウィンドウハンドル
		nullptr,				//メニューハンドル
		wndClass_.hInstance,	//インスタンスハンドル
		nullptr					//オプション
	);

	if (hWnd_ == nullptr) {
		return false;
	}
	//ウィンドウを表示する
	ShowWindow(hWnd_, SW_SHOW);
	return true;
}

bool WinApp::ProcessMessage() {
	MSG msg{};	//メッセージ

	//Windowにメッセージが来てたら最優先で処理させる
	if (PeekMessage(&msg, NULL, 0, 0, PM_REMOVE)) {
		TranslateMessage(&msg);	//キー入力メッセージの処理
		DispatchMessage(&msg);	//ウィンドウプロシージャにメッセージを送る
	}

	if (msg.message == WM_QUIT) {
		return true;
	}

	return false;
}
