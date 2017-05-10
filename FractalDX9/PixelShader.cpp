#include "stdafx.h"
#include "PixelShader.h"

#include <string>
using namespace std;

LPDIRECT3D9             g_pD3D = NULL;			// D3DDevice
LPDIRECT3DDEVICE9       g_pd3dDevice = NULL;	// Rendering device
LPDIRECT3DVERTEXBUFFER9 g_pVB = NULL;			// Buffer to hold vertices

// Our custom vertex type
struct CUSTOMVERTEX
{
	FLOAT x, y; // The transformed position for the vertex
};

// Describes our custom vertex type
#define D3DFVF_CUSTOMVERTEX (D3DFVF_XYZRHW | D3DFVF_DIFFUSE)

void PixelShader::Run(HWND hWnd)
{
	// Initialize Direct3D
	if (SUCCEEDED(InitD3D(hWnd)))
	{
		// Create the vertex buffer
		if (SUCCEEDED(InitVB()))
		{
			// Show the window
			ShowWindow(hWnd, SW_SHOWDEFAULT);
			UpdateWindow(hWnd);

			g_pd3dDevice->Clear(0, NULL, D3DCLEAR_TARGET, D3DCOLOR_XRGB(0, 0, 255), 1.0f, 0);

			// Compile and set the pixel shader

			IDirect3DPixelShader9 * g_lpPixelShader = NULL;

			LPD3DXBUFFER pCode = NULL;
			LPD3DXBUFFER pErrorMsgs = NULL;
			LPD3DXCONSTANTTABLE pConstantTable = NULL;

			HRESULT hRes = D3DXCompileShaderFromFile(L"PixelShader.ps", NULL, NULL, "PS_Main", "ps_3_0", 0, &pCode, &pErrorMsgs, &pConstantTable);

			if (FAILED(hRes))
			{
				MessageBox(NULL, L"Pixel Shader Compilation Failed", L"Compiling Pixel Shader From File - ERROR", NULL);
			}
			else
			{
				D3DXHANDLE L_PS = pConstantTable->GetConstantByName(NULL, "L");
				pConstantTable->SetInt(g_pd3dDevice, L_PS, L);

				D3DXHANDLE H_PS = pConstantTable->GetConstantByName(NULL, "H");
				pConstantTable->SetInt(g_pd3dDevice, H_PS, H);

				D3DXHANDLE div_PS = pConstantTable->GetConstantByName(NULL, "div");
				pConstantTable->SetInt(g_pd3dDevice, div_PS, div);

				D3DXHANDLE divL_PS = pConstantTable->GetConstantByName(NULL, "divL");
				pConstantTable->SetInt(g_pd3dDevice, divL_PS, divL);

				D3DXHANDLE divH_PS = pConstantTable->GetConstantByName(NULL, "divH");
				pConstantTable->SetInt(g_pd3dDevice, divH_PS, divH);

				g_pd3dDevice->CreatePixelShader((DWORD *)pCode->GetBufferPointer(), &g_lpPixelShader);
				g_pd3dDevice->SetPixelShader(g_lpPixelShader);

				Render();

				LPDIRECT3DSURFACE9 pRenderTarget = NULL;
				LPDIRECT3DSURFACE9 pDestTarget;
				LPDIRECT3DTEXTURE9 pTexture;

				// Render target
				g_pd3dDevice->GetRenderTarget(0, &pRenderTarget);

				// Dest Surface
				g_pd3dDevice->CreateTexture(L, H, 1, 0, D3DFMT_X8R8G8B8, D3DPOOL_SYSTEMMEM, &pTexture, NULL);
				pTexture->GetSurfaceLevel(0, &pDestTarget);

				// Copy the render target to the destination surface
				g_pd3dDevice->GetRenderTargetData(pRenderTarget, pDestTarget);

				// Save its contents to a bitmap file
				wchar_t ch_a[2];
				wchar_t ch_b[2];
				_itow_s(divL, ch_a, 2, 10);
				_itow_s(divH, ch_b, 2, 10);

				wstring ws_a(ch_a);
				wstring ws_b(ch_b);
				wstring filename;

				switch (fileFormatSelector)
				{
				case (0):
					filename = L"ComputingRenderedImage-" + ws_a + L"-" + ws_b + L".bmp";
					D3DXSaveSurfaceToFile(filename.c_str(), D3DXIFF_BMP, pDestTarget,	NULL, NULL);
					break;

				case (1):
					filename = L"ComputingRenderedImage-" + ws_a + L"-" + ws_b + L".jpg";
					D3DXSaveSurfaceToFile(filename.c_str(), D3DXIFF_JPG, pDestTarget,	NULL, NULL);
					break;
				}
			}
		}
	}
}

HRESULT PixelShader::InitD3D(HWND hWnd)
{
	// Create the D3D object.
	if (NULL == (g_pD3D = Direct3DCreate9(D3D_SDK_VERSION)))
	{
		return E_FAIL;
	}

	D3DDISPLAYMODE d3ddm;
	g_pD3D->GetAdapterDisplayMode(D3DADAPTER_DEFAULT, &d3ddm);

	// Set up the structure used to create the D3DDevice
	D3DPRESENT_PARAMETERS d3dpp;
	ZeroMemory(&d3dpp, sizeof(d3dpp));

	d3dpp.Windowed               = TRUE;
	d3dpp.SwapEffect             = D3DSWAPEFFECT_DISCARD;
	d3dpp.BackBufferFormat       = d3ddm.Format;
	d3dpp.BackBufferWidth		 = PixelShader::L;
	d3dpp.BackBufferHeight		 = PixelShader::H;
	d3dpp.PresentationInterval   = D3DPRESENT_INTERVAL_IMMEDIATE;

	// Create the D3DDevice
	if (FAILED(g_pD3D->CreateDevice(D3DADAPTER_DEFAULT, D3DDEVTYPE_HAL, hWnd,
	                                D3DCREATE_HARDWARE_VERTEXPROCESSING,
	                                &d3dpp, &g_pd3dDevice)))
	{
		return E_FAIL;
	}

	return S_OK;
}

HRESULT PixelShader::InitVB()
{
	// CUSTOMVERTEX Initialisation
	CUSTOMVERTEX vertices[] =
	{
		{ 0.0f, 0.0f }, // x, y
		{ float(PixelShader::L), 0.0f },
		{  0.0f, float(PixelShader::H) },
		{ float(PixelShader::L), float(PixelShader::H) }

	};


	// Create the vertex buffer
	if (FAILED(g_pd3dDevice->CreateVertexBuffer(4 * sizeof(CUSTOMVERTEX),
	                                            0, D3DFVF_CUSTOMVERTEX,
	                                            D3DPOOL_DEFAULT, &g_pVB, NULL)))
	{
		return E_FAIL;
	}

	// Fill the vertex buffer
	VOID * pVertices;

	if (FAILED(g_pVB->Lock(0, 0, (void **)&pVertices, 0)))
	{
		return E_FAIL;
	}

	memcpy(pVertices, vertices, sizeof(vertices));
	g_pVB->Unlock();

	return S_OK;
}

void PixelShader::Cleanup()
{
	if (g_pVB != NULL)
	{
		g_pVB->Release();
	}

	if (g_pd3dDevice != NULL)
	{
		g_pd3dDevice->Release();
	}

	if (g_pD3D != NULL)
	{
		g_pD3D->Release();
	}
}

void PixelShader::Render()
{
	// Clear the backbuffer
	g_pd3dDevice->Clear(0, NULL, D3DCLEAR_TARGET, D3DCOLOR_XRGB(0, 0, 255), 1.0f, 0);

	// Begin the scene
	if (SUCCEEDED(g_pd3dDevice->BeginScene()))
	{
		// Draw the triangles in the vertex buffer.
		g_pd3dDevice->SetFVF(D3DFVF_CUSTOMVERTEX);
		g_pd3dDevice->SetStreamSource(0, g_pVB, 0, sizeof(CUSTOMVERTEX));
		g_pd3dDevice->DrawPrimitive(D3DPT_TRIANGLESTRIP, 0, 2);

		// End the scene
		g_pd3dDevice->EndScene();
	}

	// Present the backbuffer contents to the display
	g_pd3dDevice->Present(NULL, NULL, NULL, NULL);
}