#pragma once
#include <Transform.h>
#include <Matrix4x4.h>

class Camera {
public:
	Camera();

	void Update();

	Matrix4x4 GetVeiwProjectionMatrix() const { return viewProjectionMatrix_; }

private:
	Matrix4x4 worldMatrix_;
	Matrix4x4 viewProjectionMatrix_;
	Matrix4x4 projectionMatrix_;
	Matrix4x4 viewMatrix_;
	Transforms transform_;
};

