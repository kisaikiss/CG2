#include "D3DResourceLeakChecker.h"
#include <wrl.h>
#include <dxgidebug.h>
#include <dxgi1_6.h>
#include <strsafe.h>
#include <d3d12.h>

#pragma comment(lib,"d3d12.lib")
#pragma comment(lib,"dxguid.lib")
#pragma comment(lib,"dxgi.lib")

D3DResourceLeakChecker::~D3DResourceLeakChecker() {
	Microsoft::WRL::ComPtr<IDXGIDebug1> debug;
	if (SUCCEEDED(DXGIGetDebugInterface1(0, IID_PPV_ARGS(&debug)))) {
		debug->ReportLiveObjects(DXGI_DEBUG_ALL, DXGI_DEBUG_RLO_ALL);
		debug->ReportLiveObjects(DXGI_DEBUG_APP, DXGI_DEBUG_RLO_ALL);
		debug->ReportLiveObjects(DXGI_DEBUG_D3D12, DXGI_DEBUG_RLO_ALL);
	}
}
