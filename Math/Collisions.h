#pragma once
struct AABB;

/// <summary>
/// AABB同士の当たり判定
/// </summary>
/// <param name="aabb"></param>
/// <param name="aabb2"></param>
/// <returns></returns>
bool IsCollision(const AABB& aabb, const AABB& aabb2);