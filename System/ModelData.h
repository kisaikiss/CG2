#pragma once
#include <vector>
#include "VertexData.h"
#include "MaterialData.h"
#include <string>

struct ModelData {
	std::vector<VertexData> vertices;
	MaterialData material;
	std::string name;
};