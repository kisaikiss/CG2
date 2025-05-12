#pragma once
#define NOMINMAX
#include <Transform.h>
#include <Matrix4x4.h>


class Camera {
public:
	Camera();

	virtual void Update();

	Matrix4x4 GetVeiwProjectionMatrix() const { return viewProjectionMatrix_; }


	void SetPosition(const Vector3& position) { transform_.translate = position; }
	Vector3 GetPosition() const { return transform_.translate; }
protected:
	Matrix4x4 worldMatrix_;
	Matrix4x4 viewProjectionMatrix_;
	Matrix4x4 projectionMatrix_;
	Matrix4x4 viewMatrix_;
	Transforms transform_;
};

