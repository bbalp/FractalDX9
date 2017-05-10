#pragma once

#include <d3d9.h>
#include <d3dx9.h>
#if defined(_DEBUG)
#pragma comment(lib, "D3d9.lib")
#pragma comment(lib, "D3dx9d.lib")
#else
#pragma comment(lib, "D3d9.lib")
#pragma comment(lib, "D3dx9.lib")
#endif

class PixelShader
{
public:
	int L;
	int H;

	int div;
	int divL;
	int divH;

	int fileFormatSelector;

	void Run(HWND hWnd);
	void Cleanup();

private:
	HRESULT InitD3D(HWND hWnd);
	HRESULT InitVB();
	void Render();
};