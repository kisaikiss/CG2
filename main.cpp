#include <dxgidebug.h>
#include <DbgHelp.h>
#include <strsafe.h>

#include "Engine.h"
#include "Log.h"
#include "Sphere.h"
#include "Triangle.h"
#include "Camera.h"
#include "Sprite.h"
#include <Model.h>
#include "Input.h"
#include "DebugCamera.h"
#include "D3DResourceLeakChecker.h"
#include "Audio.h"

#include "imgui.h"
#include "imgui_impl_dx12.h"
#include "imgui_impl_win32.h"

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

	D3DResourceLeakChecker leakCheck;
	Logger* log = new Logger();
	log->LogInit();
	delete log;
	Engine* engine = new Engine();
	engine->Initialize();

	/*--------------------------------------------*/
	/*理由がない限りここより上には追加で何も書くな*/
	/*--------------------------------------------*/

	Audio* audio = new Audio();

	uint32_t soundNum = audio->SoundLoadWave("resources/Alarm01.wav");
	audio->SoundPlayWave(soundNum);
	
	Sphere* sphere = new Sphere(engine);
	Triangle* triangle1 = new Triangle(engine);
	Camera* camera = new Camera();
	Sprite* sprite = new Sprite(engine,"resources/uvChecker.png");
	Model* model = new Model(engine,"resources","player.obj");
	Model* debugModel = new Model(engine, "resources", "player.obj");
	Transforms transform{};
	transform.scale = { -1.05f,1.05f,1.05f };
	debugModel->SetColor({ 0.f, 0.f, 0.f, 1.f });
	debugModel->SetTransform(transform);
	Input* input = new Input();
	DebugCamera* debugCamera = new DebugCamera(input);

	Camera* rialCamera = camera;

	bool isDebugCamera = false;

	input->Initialize(engine->GetHWND());

	//ウィンドウのxボタンが押されるまでループ
	while (engine->ProcessMessage() == 0) {
		//ゲームの処理
		engine->FrameStart();

		/*------------------------*/
		/*↓↓更新処理ここから↓↓*/
		/*------------------------*/

		input->Update();
		sphere->Update();
		triangle1->Update();
		model->Update();
		debugModel->Update();
		sprite->Update();

		if (input->GetPressingCount('0') == 1) {
			if (isDebugCamera == true) {
				isDebugCamera = false;
			} else {
				isDebugCamera = true;
			}
		}
		if (isDebugCamera == true) {
			debugCamera->Update();
			rialCamera = debugCamera;
		} else {
			camera->Update();
			rialCamera = camera;
		}
		
		engine->UpdateLight();
		/*------------------------*/
		/*↑↑更新処理ここまで↑↑*/
		/*------------------------*/

		/*------------------------*/
		/*↓↓描画処理ここから↓↓*/
		/*------------------------*/

		engine->PreDraw();

		triangle1->Draw(*rialCamera);
		sphere->Draw(*rialCamera);
		model->Draw(*rialCamera);
		debugModel->Draw(*rialCamera, transform);
		Vector2 pos = { 2.f,3.f };
		Vector2 size = { 100.f,500.f };

		sprite->Draw();

		engine->PostDraw();
		/*------------------------*/
		/*↑↑描画処理ここまで↑↑*/
		/*------------------------*/
	}
	delete audio;
	delete sphere;
	delete sprite;
	delete triangle1;
	delete camera;
	delete debugCamera;
	delete model;
	delete debugModel;
	delete input;
	engine->Finalize();
	delete engine;
	
	CoUninitialize();
	return 0;
}