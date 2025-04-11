#include <dxgidebug.h>

#include "Adapter.h"

//Windowsアプリでのエントリーポイント(main関数)
int WINAPI WinMain(HINSTANCE, HINSTANCE, LPSTR, int) {

	//COM初期化
	CoInitializeEx(0, COINIT_MULTITHREADED);

	Adapter* adapter = new Adapter();
	adapter->Initialize();

	//ウィンドウのxボタンが押されるまでループ
	while (adapter->ProcessMessage() == 0) {
		//ゲームの処理
		adapter->FrameStart();
		//ImGui::ShowDemoWindow();

		adapter->UpdateTriangle();

		adapter->PreDraw();

		adapter->DrawTriangle();

		adapter->PostDraw();
	}

	//出力ウィンドウへの文字出力
	OutputDebugStringA("Hello, DirectX!!\n");

	adapter->Finalize();
	delete adapter;
	CoUninitialize();
	return 0;
}