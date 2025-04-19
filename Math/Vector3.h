#pragma once
class Vector3 {
public:
	float x, y, z;

	Vector3 operator-(const Vector3& other) const;

	Vector3 operator+(const Vector3& other) const;

	Vector3 operator+(const float& scalar) const;
};

