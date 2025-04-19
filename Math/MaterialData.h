#pragma once
#include "Vector4.h"
#include <cstdint>

struct MaterialData {
	Vector4 color;
	int32_t enableLighting;
};