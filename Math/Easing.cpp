#include "Easing.h"
#include <cmath>
#include <numbers>

float EaseInOut(float change, float base, float duration, float time) {
	time /= duration;
	if (time > 1.f || time < 0.f) {
		return change;
	}
	float easedT = -(std::cos(static_cast<float>(std::numbers::pi_v<float>) * time) - 1.0f) / 2.0f;
	return (1.0f - easedT) * base + easedT * change;
};