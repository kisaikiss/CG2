#include "Log.h"
#include <dxgidebug.h>

void Log(const std::string& message) {
	OutputDebugStringA(message.c_str());
}