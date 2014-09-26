////////////////////////////////////////////////////////////////
// File: d3d.cpp
// Desc: D3D management and utilities class
////////////////////////////////////////////////////////////////
#ifndef __D3D_H__
#define __D3D_H__

#include "stddef.h"
#include <d3dx9.h>

#pragma comment(lib, "d3d9.lib")
#pragma comment(lib, "d3dx9.lib")

D3DMATERIAL9 CreateMaterial(const D3DXCOLOR& ambient, const D3DXCOLOR& diffuse, 
							const D3DXCOLOR& specular, const D3DXCOLOR& emissive, float power);

D3DLIGHT9 CreateDirectionalLight(const D3DXVECTOR3& direction, const D3DXCOLOR& color);

D3DLIGHT9 CreatePointLight(const D3DXVECTOR3& direction, const D3DXCOLOR& color);

D3DLIGHT9 CreateSpotLight(const D3DXVECTOR3& position, const D3DXVECTOR3& direction,
						  const D3DXCOLOR& color);

const D3DXCOLOR WHITE( D3DCOLOR_XRGB(255, 255, 255) );
const D3DXCOLOR BLACK( D3DCOLOR_XRGB( 0, 0, 0) );
const D3DXCOLOR RED( D3DCOLOR_XRGB(255, 0, 0) );
const D3DXCOLOR GREEN( D3DCOLOR_XRGB( 0, 255, 0) );
const D3DXCOLOR BLUE( D3DCOLOR_XRGB( 0, 0, 255) );
const D3DXCOLOR YELLOW( D3DCOLOR_XRGB(255, 255, 0) );
const D3DXCOLOR CYAN( D3DCOLOR_XRGB( 0, 255, 255) );
const D3DXCOLOR MAGENTA( D3DCOLOR_XRGB(255, 0, 255) );
const D3DMATERIAL9 WHITE_MTRL = CreateMaterial(WHITE, WHITE, WHITE, BLACK, 8.0f);
const D3DMATERIAL9 RED_MTRL = CreateMaterial(RED, RED, RED, BLACK, 8.0f);
const D3DMATERIAL9 GREEN_MTRL = CreateMaterial(GREEN, GREEN, GREEN, BLACK, 8.0f);
const D3DMATERIAL9 BLUE_MTRL = CreateMaterial(BLUE, BLUE, BLUE, BLACK, 8.0f);
const D3DMATERIAL9 YELLOW_MTRL = CreateMaterial(YELLOW, YELLOW, YELLOW, BLACK, 8.0f);
const D3DMATERIAL9 CYAN_MTRL = CreateMaterial(CYAN, CYAN, CYAN, BLACK, 8.0f);
const D3DMATERIAL9 MAGENTA_MTRL = CreateMaterial(MAGENTA, MAGENTA, MAGENTA, BLACK, 8.0f);

class CD3D
{
public:
	CD3D(HWND hWnd, int width, int height, bool fullscreen);
	~CD3D();

	IDirect3DDevice9* getDevice() const { return mDevice; }
	CD3DFont* getFont() const { return mFont; }

private:
	//Create Direct3D
	void createDirect3D();
	void destroyDirect3D();

	//Direct3D Objects
	LPDIRECT3DDEVICE9		mDevice;
	D3DCAPS9				mDeviceCaps;
	CD3DFont*				mFont;

	//Handles/Objects
	HWND					m_hWnd;

	int						m_iWidth;
	int						m_iHeight;
	bool					m_bFullscreen;
};

#endif