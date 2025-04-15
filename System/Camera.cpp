#include "Camera.h"
#include "MatrixCalculations.h"
#include "imgui.h"
#include "imgui_impl_dx12.h"
#include "imgui_impl_win32.h"

Camera::Camera() {
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

void Camera::Update() {
	ImGui::Begin("camera");
	ImGui::DragFloat3("position", &transform_.translate.x, 0.01f);
	ImGui::DragFloat3("rotate", &transform_.rotate.x, 0.01f);
	ImGui::DragFloat3("scale", &transform_.scale.x, 0.01f);
	ImGui::End();

	worldMatrix_ = MakeAffineMatrix(transform_.scale, transform_.rotate, transform_.translate);
	viewMatrix_ = Inverse(worldMatrix_);
	viewProjectionMatrix_ = Multiply(viewMatrix_, projectionMatrix_);
}
