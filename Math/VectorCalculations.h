#pragma once
#pragma once
#include "Vector3.h"
#include <cmath>
#include <cassert>

/// <summary>
/// ベクトルの内積
/// </summary>
/// <param name="v1"></param>
/// <param name="v2"></param>
/// <returns></returns>
float Dot(Vector3 v1, Vector3 v2);

/// <summary>
/// ベクトルの外積
/// </summary>
/// <param name="v1"></param>
/// <param name="v2"></param>
/// <returns></returns>
Vector3 Cross(Vector3& v1, Vector3& v2);

/// <summary>
/// ベクトルの長さ
/// </summary>
/// <param name="vector3"></param>
/// <returns></returns>
float Length(Vector3 vector3);

/// <summary>
/// ベクトルの正規化
/// </summary>
/// <param name="vector3"></param>
/// <returns></returns>
Vector3 Normalize(Vector3& vector3);

/// <summary>
/// 正射影ベクトル(v1をv2に投影)
/// </summary>
/// <param name="v1"></param>
/// <param name="v2"></param>
/// <returns></returns>
Vector3 MakeOrthographicVector(Vector3& v1, Vector3& v2);