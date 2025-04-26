#include "DebugCamera.h"
#include "MatrixCalculations.h"

#include "imgui.h"
#include "imgui_impl_dx12.h"
#include "imgui_impl_win32.h"

DebugCamera::DebugCamera(Input* input) {
	if (input == nullptr) {
		assert(0);
	}

	input_ = input;
	transform_ = { {1.f,1.f,1.f},{0.f,0.f,0.f},{0.f,0.f,-5.f} };
	worldMatrix_ = MakeAffineMatrix(transform_.scale, transform_.rotate, transform_.translate);
	viewMatrix_ = Inverse(worldMatrix_);
	projectionMatrix_ = MakePerspectiveFovMatrix(
		0.45f,
		1280.f / 720.f,
		0.1f,
		100.f
	);
	viewProjectionMatrix_ = Multiply(viewMatrix_, projectionMatrix_);
}

void DebugCamera::Update() {
	if (input_->IsPressMouse(1)) {
		
		if (input_->IsTriggerMouse(1)) {
			mousePositionX_ = 0;
			mousePositionY_ = 0;
			preMousePositionX_ = mousePositionX_;
			preMousePositionY_ = mousePositionY_;
		} else {
			preMousePositionX_ = mousePositionX_;
			preMousePositionY_ = mousePositionY_;
			input_->GetMousePosition(&mousePositionX_, &mousePositionY_);
		}
		
		Vector3 CameraRotate{};
		if (preMousePositionX_ && mousePositionX_) {
			//カメラの回転はXとY反転させた方が直感的
			CameraRotate.x = static_cast<float>(mousePositionY_ - preMousePositionY_) / 200.f;
			CameraRotate.y = static_cast<float>(mousePositionX_ - preMousePositionX_) / 200.f;
		}
		
		//移動速度
		const float kSpeed = 0.1f;
		Vector3 velocity{};

		

		transform_.rotate = transform_.rotate + CameraRotate;
		if (input_->GetPressingCount('W')) {
			velocity.z += kSpeed;
		}
		if (input_->GetPressingCount('A')) {
			velocity.x -= kSpeed;
		}
		if (input_->GetPressingCount('S')) {
			velocity.z -= kSpeed ;
		}
		if (input_->GetPressingCount('D')) {
			velocity.x += kSpeed;
		}

		//速度ベクトルを自機の向きに合わせて回転させる
		velocity = TransforNormal(velocity, worldMatrix_);

		//フラグが立っていなければ向いている方向に合わせてy軸方向には移動しない
		if (!isYMove_) {
			velocity.y = 0.0f;
		}

		if (input_->GetPressingCount('E')) {
			velocity.y += kSpeed;
		}
		if (input_->GetPressingCount('Q')) {
			velocity.y -= kSpeed;
		}
		transform_.translate = transform_.translate + velocity;
	}



	worldMatrix_ = MakeAffineMatrix(transform_.scale, transform_.rotate, transform_.translate);
	viewMatrix_ = Inverse(worldMatrix_);
	viewProjectionMatrix_ = Multiply(viewMatrix_, projectionMatrix_);
	ImGui::Begin("debugCamera");
	ImGui::DragFloat3("position", &transform_.translate.x, 0.01f);
	ImGui::DragFloat3("rotate", &transform_.rotate.x, 0.01f);
	ImGui::Checkbox("isYMove", &isYMove_);
	ImGui::End();
}
