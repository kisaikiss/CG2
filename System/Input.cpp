#include "Input.h"
#include <cassert>

void Input::Initialize(HWND hwnd) {
	if (hwnd == nullptr) {
		assert(0);
	}
	hwnd_ = hwnd;
	result_ = GameInputCreate(input_.GetAddressOf());
	assert(SUCCEEDED(result_));
}

void Input::Update() {
#pragma region キーボード入力
	// 押す状態をリセット
	preMouseState_ = mouseState_;

	IGameInputReading* reading = nullptr;
	if (SUCCEEDED(input_->GetCurrentReading(GameInputKindKeyboard, nullptr, &reading))) {
		bool keys[kKeyNum] = {};
		// 押すキーの数
		uint32_t keyCount = reading->GetKeyCount();
		keyStates_.resize(keyCount);
		if (keyCount > 0) {
			// 押すキー状態
			reading->GetKeyState(keyCount, keyStates_.data());

			for (uint32_t i = 0; i < keyCount; i++) {
				uint8_t virtualKey = keyStates_[i].virtualKey;
				keys[virtualKey] = true;
			}
		}
		for (uint32_t i = 0; i < kKeyNum; i++) {
			if (keys[i] != 0) {            //i番のキーが押されていたら
				if (keRreleasingCount_[i] > 0) {//離されカウンタが0より大きければ
					keRreleasingCount_[i] = 0;   //0に戻す
				}
				keyPressingCount_[i]++;          //押されカウンタを増やす
			} else {                             //i番のキーが離されていたら
				if (keyPressingCount_[i] > 0) { //押されカウンタが0より大きければ
					keyPressingCount_[i] = 0;    //0に戻す
				}
				keRreleasingCount_[i]++;         //離されカウンタを増やす
			}
		}
		reading->Release();
	}



#pragma endregion

#pragma region マウス入力
	IGameInputReading* readingMouse = nullptr;
	if (SUCCEEDED(input_->GetCurrentReading(GameInputKindMouse, nullptr, &readingMouse))) {
		readingMouse->GetMouseState(&mouseState_);
		readingMouse->Release();
	}
#pragma endregion
}

bool Input::IsPressMouse(int button)const {
	if(button < 0 || button >= NONE)return false;
	switch (button) {
	case MouseButton::Left:
		return mouseState_.buttons & GameInputMouseLeftButton;
	case MouseButton::Right:
		return mouseState_.buttons & GameInputMouseRightButton;
	case MouseButton::Middle:
		return mouseState_.buttons & GameInputMouseMiddleButton;
	}
	return false;
}

bool Input::IsTriggerMouse(int button)const {
	if (button < 0 || button >= NONE) return false;

	switch (button) {
	case MouseButton::Left:
		return (mouseState_.buttons & GameInputMouseLeftButton) &&
			!(preMouseState_.buttons & GameInputMouseLeftButton);
	case MouseButton::Right:
		return (mouseState_.buttons & GameInputMouseRightButton) &&
			!(preMouseState_.buttons & GameInputMouseRightButton);
	case MouseButton::Middle:
		return (mouseState_.buttons & GameInputMouseMiddleButton) &&
			!(preMouseState_.buttons & GameInputMouseMiddleButton);
	}
	return false;
}

int Input::GetPressingCount(uint8_t keyCode) {
	return keyPressingCount_[keyCode];
}

int Input::GetReleasingCount(uint8_t keyCode) {
	return keRreleasingCount_[keyCode];
}

bool Input::GetMousePosition(int* mouseX, int* mouseY) {
	if (mouseX && mouseY) {
		// スクリーンを座標系からウィンドウ座標に変換
		POINT cursorPos;
		GetCursorPos(&cursorPos); // スクリーンを得る
		ScreenToClient(hwnd_, &cursorPos); // ウィンドウ座標に変換

		*mouseX = static_cast<int>(cursorPos.x);
		*mouseY = static_cast<int>(cursorPos.y);
		return true;
	}

	return false;
}