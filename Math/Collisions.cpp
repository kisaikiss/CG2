#include "Collisions.h"
#include "AABB.h"

bool IsCollision(const AABB& aabb, const AABB& aabb2) {
	if ((aabb.min.x <= aabb2.max.x && aabb.max.x >= aabb2.min.x) &&
		(aabb.min.y <= aabb2.max.y && aabb.max.y >= aabb2.min.y) &&
		(aabb.min.z <= aabb2.max.z && aabb.max.z >= aabb2.min.z)) {
		return true;
	}
	return false;
}
