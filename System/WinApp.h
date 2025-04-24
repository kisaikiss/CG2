#pragma once
#include <Windows.h>
#include <cstdint>
#include <string>
#include "imgui.h"
#include "imgui_impl_dx12.h"
#include "imgui_impl_win32.h"
extern IMGUI_IMPL_API LRESULT ImGui_ImplWin32_WndProcHandler(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam);

/// <summary>
/// ウィンドウズアプリケーション
/// </summary>
class WinApp {
public:
	// ウィンドウサイズ
	static const int kWindowWidth = 1280; // 横幅
	static const int kWindowHeight = 720; // 縦幅

	/// <summary>
	/// ウィンドウの作成
	/// </summary>
	/// <param name="clientWidth">クライアント領域の横幅</param>
	/// <param name="clientHeight">クライアント領域の縦幅</param>
	/// <param name="titleName">タイトル名</param>
	/// <param name="windowClassName">クラス名</param>
	/// <returns>成功したらtrue</returns>
	bool Create(const std::wstring& titleName, const std::wstring& windowClassName,
		int32_t clientWidth = kWindowWidth, int32_t clientHeight = kWindowHeight);

	bool ProcessMessage();

	/// <summary>
	/// ウィンドウハンドルの取得
	/// </summary>
	/// <returns></returns>
	HWND GetHWND() const { return hWnd_; }

	/// <summary>
	/// ウィンドウクラスの取得
	/// </summary>
	/// <returns></returns>
	WNDCLASS GetWndClass() const { return wndClass_; }
private:
	HWND hWnd_ = nullptr;	//ウィンドウハンドル
	WNDCLASS wndClass_{};
};

LRESULT CALLBACK WindowProc(HWND hwnd, UINT msg, WPARAM wpram, LPARAM lparam);