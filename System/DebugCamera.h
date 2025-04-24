#pragma once
#include "Transform.h"
#include "Matrix4x4.h"
#include "Camera.h"
#include "Input.h"

class DebugCamera : public Camera {
public:
	DebugCamera(Input* input);

	void Update() override;
private:
	int preMousePositionX_ = 0;
	int preMousePositionY_ = 0;
	int mousePositionX_ = 0;
	int mousePositionY_ = 0;
	//XZ軸方向移動の時に向いている方向に合わせてY軸方向の移動もするか
	bool isYMove_ = true;
	Input* input_;
};

