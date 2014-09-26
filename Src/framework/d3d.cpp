////////////////////////////////////////////////////////////////
// File: d3d.cpp
// Desc: D3D management and utilities class
////////////////////////////////////////////////////////////////
#include "d3d.h"
//#include "application.h"

//CD3D Implmentation
CD3D::CD3D(HWND hWnd, int width, int height, bool fullscreen) : m_hWnd(hWnd),
	m_iWidth(width), m_iHeight(height), m_bFullscreen(fullscreen), mFont(NULL)
{
	assert(m_hWnd != NULL);
	createDirect3D();
}

CD3D::~CD3D()
{
	destroyDirect3D();
}

void CD3D::createDirect3D()
{
	HRESULT hr;

	//Create Direct3D COM object
	LPDIRECT3D9 d3d = Direct3DCreate9(D3D_SDK_VERSION);
	if( !d3d )
	{
		throw CException(L"Direct3DCreate9 failed", LINE_AND_FILE);
	}

	//Get device capabilities
	d3d->GetDeviceCaps(D3DADAPTER_DEFAULT, D3DDEVTYPE_HAL, &mDeviceCaps);

	int vp = 0;
	if( mDeviceCaps.DevCaps & D3DDEVCAPS_HWTRANSFORMANDLIGHT )
	{
		vp = D3DCREATE_HARDWARE_VERTEXPROCESSING;
	}
	else
	{
		vp = D3DCREATE_SOFTWARE_VERTEXPROCESSING;
	}

	//Create the Direct3D device
	D3DPRESENT_PARAMETERS d3dpp;
	d3dpp.BackBufferWidth            = m_iWidth;
	d3dpp.BackBufferHeight           = m_iHeight;
	d3dpp.BackBufferFormat           = D3DFMT_A8R8G8B8;
	d3dpp.BackBufferCount            = 1;
	d3dpp.MultiSampleType            = D3DMULTISAMPLE_NONE;
	d3dpp.MultiSampleQuality         = 0;
	d3dpp.SwapEffect                 = D3DSWAPEFFECT_DISCARD; 
	d3dpp.hDeviceWindow              = m_hWnd;
	d3dpp.Windowed                   = !m_bFullscreen;
	d3dpp.EnableAutoDepthStencil     = true; 
	d3dpp.AutoDepthStencilFormat     = D3DFMT_D24S8;
	d3dpp.Flags                      = 0;
	d3dpp.FullScreen_RefreshRateInHz = D3DPRESENT_RATE_DEFAULT;
	d3dpp.PresentationInterval       = D3DPRESENT_INTERVAL_IMMEDIATE;

	hr = d3d->CreateDevice(D3DADAPTER_DEFAULT, D3DDEVTYPE_HAL, m_hWnd, vp,
		&d3dpp, &mDevice);
	if(FAILED(hr))
	{
		SAFERELEASE(d3d);
		throw CException(TranslateHResult(hr), LINE_AND_FILE);
	}

	SAFERELEASE(d3d);

	//Create default font
	mFont = new CD3DFont(L"Courier New", 9, 0);
	hr = mFont->InitDeviceObjects(mDevice);
	if(FAILED(hr))
	{
		throw CException(TranslateHResult(hr), LINE_AND_FILE);
	}
	mFont->RestoreDeviceObjects();
	if(FAILED(hr))
	{
		throw CException(TranslateHResult(hr), LINE_AND_FILE);
	}
}

void CD3D::destroyDirect3D()
{
	mFont->InvalidateDeviceObjects();
	mFont->DeleteDeviceObjects();
	SAFEDELETE(mFont);
	SAFERELEASE(mDevice);
}

// Functions
D3DMATERIAL9 CreateMaterial(const D3DXCOLOR& ambient, const D3DXCOLOR& diffuse, 
	const D3DXCOLOR& specular, const D3DXCOLOR& emissive, float power)
{
	D3DMATERIAL9 mtrl;
	mtrl.Ambient = ambient;
	mtrl.Diffuse = diffuse;
	mtrl.Specular = specular;
	mtrl.Emissive = emissive;
	mtrl.Power = power;
	return mtrl;
}

D3DLIGHT9 CreateDirectionalLight(const D3DXVECTOR3& direction, const D3DXCOLOR& color)
{
	D3DLIGHT9 light;
	ZeroMemory(&light, sizeof(light));
	light.Type = D3DLIGHT_DIRECTIONAL;
	light.Ambient = color * 0.4f;
	light.Diffuse = color;
	light.Specular = color * 0.6f;
	light.Direction = direction;
	return light;
}

D3DLIGHT9 CreatePointLight(const D3DXVECTOR3& position, const D3DXCOLOR& color)
{
	D3DLIGHT9 light;
	ZeroMemory(&light, sizeof(light));
	light.Type			= D3DLIGHT_POINT;
	light.Ambient		= color * 0.6f;
	light.Diffuse		= color;
	light.Specular		= color * 0.6f;
	light.Position		= position;
	light.Range			= 1000.0f;
	light.Falloff		= 1.0f;
	light.Attenuation0	= 1.0f;
	light.Attenuation1	= 0.0f;
	light.Attenuation2	= 0.0f;
	return light;
}

D3DLIGHT9 CreateSpotLight(const D3DXVECTOR3& position, const D3DXVECTOR3& direction, 
	const D3DXCOLOR& color)
{
	D3DLIGHT9 light;
	ZeroMemory(&light, sizeof(light));
	light.Type			= D3DLIGHT_SPOT;
	light.Ambient		= color * 0.0f;
	light.Diffuse		= color;
	light.Specular		= color * 0.6f;
	light.Position		= position;
	light.Direction		= direction;
	light.Range			= 1000.0f;
	light.Falloff		= 1.0f;
	light.Attenuation0	= 1.0f;
	light.Attenuation1	= 0.0f;
	light.Attenuation2	= 0.0f;
	light.Theta			= 0.4f;
	light.Phi			= 0.9f;
	return light;
}