#pragma once
#include<GameInput.h>
#include<array>
#include <wrl/client.h>
#include <vector>
#include <Windows.h>

enum MouseButton {
	Left = 0,
	Right,
	Middle,
	NONE
};

#pragma comment(lib, "gameinput.lib")

class Input {
public:

	void Initialize(HWND hwnd);
	void Update();
	/// <summary>
	/// マウスの押下をチェック
	/// </summary>
	/// <param name="button">マウスボタン番号(0:左,1:右,2:中,3~4:拡張マウスボタン)</param>
	/// <returns>押されているか</returns>
	bool IsPressMouse(int button)const;
	/// <summary>
	/// マウスのトリガーをチェック。押した瞬間だけtrueになる
	/// </summary>
	/// <param name="button">マウスボタン番号(0:左,1:右,2:中,3~4:拡張マウスボタン)</param>
	/// <returns>トリガーか</returns>
	bool IsTriggerMouse(int button)const;

	/// <summary>
	/// キーがどれだけ押されているか
	/// </summary>
	/// <param name="keyCode">キー番号</param>
	/// <returns>押されたフレーム数</returns>
	int GetPressingCount(uint8_t keyCode);

	/// <summary>
	/// キーがどれだけ離されているか
	/// </summary>
	/// <param name="keyCode">キー番号</param>
	/// <returns>押されていないフレーム数</returns>
	int GetReleasingCount(uint8_t keyCode);

	/// <summary>
	///	マウスの位置を取得
	/// </summary>
	/// <param name="mouseX"></param>
	/// <param name="mouseY"></param>
	/// <returns></returns>
	bool GetMousePosition(int* mouseX, int* mouseY);
	/// <summary>
	/// マウスの状態を取得
	/// </summary>
	/// <returns>成功したか</returns>
	const GameInputMouseState& GetMouseState()const { return mouseState_; };

private:
	HRESULT result_ = S_FALSE;
	Microsoft::WRL::ComPtr<IGameInput> input_;
	GameInputMouseState mouseState_;
	GameInputMouseState preMouseState_;
	std::vector<GameInputKeyState> keyStates_;

	HWND hwnd_ = nullptr;

	//キーの数
	static inline const uint16_t kKeyNum = 256;

	int keyPressingCount_[kKeyNum] = {};
	int keRreleasingCount_[kKeyNum] = {};
};