#include <dxgidebug.h>
#include <DbgHelp.h>
#include <strsafe.h>

#include "Engine.h"
#include "Log.h"
#include "Sphere.h"
#include "Triangle.h"
#include "Camera.h"

#pragma comment(lib,"Dbghelp.lib")

static LONG WINAPI ExportDump(EXCEPTION_POINTERS* exception) {
	//時刻を取得して、時刻を名前に入れたファイルを作成。Dumpsディレクトリ以下に出力
	SYSTEMTIME time;
	GetLocalTime(&time);
	wchar_t filePath[MAX_PATH] = { 0 };
	CreateDirectory(L"./Dumps", nullptr);
	StringCchPrintfW(filePath, MAX_PATH, L"./Dumps/%04d-%02d%02d-%02d%02d.dmp", time.wYear, time.wMonth, time.wDay, time.wHour, time.wMinute);
	HANDLE dumpFileHandle = CreateFile(filePath, GENERIC_READ | GENERIC_WRITE, FILE_SHARE_WRITE | FILE_SHARE_READ, 0, CREATE_ALWAYS, 0, 0);
	// processId(このexeのId)とクラッシュ(例外)の発生したthreadIdを取得
	DWORD processId = GetCurrentProcessId();
	DWORD threadId = GetCurrentThreadId();
	//設定情報を入力
	MINIDUMP_EXCEPTION_INFORMATION minidumpInformation{ 0 };
	minidumpInformation.ThreadId = threadId;
	minidumpInformation.ExceptionPointers = exception;
	minidumpInformation.ClientPointers = TRUE;
	// Dumpを出力。MiniDumpNormalは最低限の情報を出力するフラグ
	MiniDumpWriteDump(GetCurrentProcess(), processId, dumpFileHandle, MiniDumpNormal, &minidumpInformation, nullptr, nullptr);
	// 他に関連づけられているSEH例外ハンドラがあれば実行。通常はプロセスを終了する
	return EXCEPTION_EXECUTE_HANDLER;
}

//Windowsアプリでのエントリーポイント(main関数)
int WINAPI WinMain(HINSTANCE, HINSTANCE, LPSTR, int) {
	// 誰も捕捉しなかった場合に(Unhandled)、捕捉する関数を登録
	// main関数が始まってすぐに登録する
	SetUnhandledExceptionFilter(ExportDump);

	//COM初期化
	CoInitializeEx(0, COINIT_MULTITHREADED);
	Logger::LogInit();
	Sphere* sphere = new Sphere();
	//Sphere* sphere2 = new Sphere();
	Triangle* triangle1 = new Triangle();
	Triangle* triangle2 = new Triangle();
	Camera* camera = new Camera();
	Engine* engine = new Engine();

	engine->Initialize();
	sphere->Initialize(engine->GetDevice(), engine->GetCommandList(),engine->GetTextureSystem());
	//sphere2->Initialize(engine->GetDevice(), engine->GetCommandList(), engine->GetTextureSystem());
	triangle1->Initialize(engine->GetDevice(), engine->GetCommandList());
	triangle2->Initialize(engine->GetDevice(), engine->GetCommandList());;

	Vector3 kTriangle2Vertex[3] = {
		{ -0.5f,-0.5f,0.5f },
		{0.0f, 0.0f, 0.0f},
		{0.5f,-0.5f,-0.5f}
	};
	triangle2->SetVertex(kTriangle2Vertex);

	//ウィンドウのxボタンが押されるまでループ
	while (engine->ProcessMessage() == 0) {
		//ゲームの処理
		engine->FrameStart();
		//ImGui::ShowDemoWindow();

		engine->UpdateSprite();
		sphere->Update();
		//sphere2->Update();
		triangle1->Update();
		triangle2->Update();
		camera->Update();
		engine->UpdateLight();


		engine->PreDraw();

		//engine->DrawTriangle();
		triangle1->Draw(*camera);
		triangle2->Draw(*camera);
		sphere->Draw(*camera);
		//sphere2->Draw(*camera);

		engine->DrawSprite();

		engine->PostDraw();
	}

	//出力ウィンドウへの文字出力
	OutputDebugStringA("Hello, DirectX!!\n");

	delete sphere;
	//delete sphere2;
	delete triangle1;
	delete triangle2;
	delete camera;
	engine->Finalize();
	delete engine;
	CoUninitialize();
	return 0;
}