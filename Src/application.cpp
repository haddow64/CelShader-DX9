////////////////////////////////////////////////////////////////
//Graeme Haddow: 0600720
//File: application.cpp
// Desc: Application class
////////////////////////////////////////////////////////////////
#include "application.h"
#include "framework/win.h"
#include "framework/d3d.h"

bool CApplication::isRunning = true;
float yaw = 0.0f, pitch = 0.0f, zoom = -16.0f;

CApplication::CApplication() : m_iWidth(1024), m_iHeight(768), m_bFullscreen(false),
	mDevice(NULL), m_strAppName(L"Cel Shading"), mD3D(NULL), mWin(NULL),
	m_uCurrentMesh(0), m_bShowHelp(false), m_bRenderOutlines(true), m_eShadingMode(E_cel_SHADING)
{
}

CApplication::CApplication(const wchar_t* applicationTitle) : m_iWidth(1024), 
	m_iHeight(768), m_bFullscreen(false), mDevice(NULL), 
	m_strAppName(applicationTitle), mD3D(NULL), mWin(NULL)
{
}

CApplication::CApplication(const wchar_t* applicationTitle, int width, int height, 
	bool fullscreen) : m_iWidth(width),	m_iHeight(height), m_bFullscreen(fullscreen),
	m_strAppName(applicationTitle), mDevice(NULL), mD3D(NULL), mWin(NULL)
{
}

CApplication::~CApplication()
{
	destroy();
	SAFEDELETE(mWin);
	SAFEDELETE(mD3D);
}

void CApplication::start()
{
	// Create Windows Object
	mWin = new CWindows(this);

	mD3D = new CD3D(mWin->getHWND(), m_iWidth, m_iHeight, m_bFullscreen);
	mDevice = mD3D->getDevice();

	// Nullify Application Objects
	appTexture = NULL;
	appVertexShader = NULL;
	appConstTable = NULL;
	mVertexDeclaration = NULL;

	// Create application
	create();

	// Start application loop
	mWin->go();
}

void CApplication::create()
{
	if(mDevice)
	{
		HRESULT hr;
		LPD3DXBUFFER vshaderBuffer = NULL, errorBuffer = NULL;

		//Load meshes
		mMesh [0] = XMeshCarcel(mDevice, L"Models/car.x");
		mMesh [1] = XMeshCarcel(mDevice, L"Models/teapot.x");
		mMesh [2] = XMeshCarcel(mDevice, L"Models/dino.x");
		mMesh [3] = XMeshCarcel(mDevice, L"Models/spaceship 2.x");
		mMesh [4] = XMeshCarcel(mDevice, L"Models/cube.x");
		mMesh [5] = XMeshCarcel(mDevice, L"Models/sphere.x");

		// Setup matrices
		D3DXMatrixPerspectiveFovLH(&mProjMat, 
			D3DX_PI * 0.25f, (float)m_iWidth / (float)m_iHeight,
			0.1f, 1000.0f);
		D3DXMatrixLookAtLH(&mViewMat, 
			&D3DXVECTOR3(0.0f, 0.8f, zoom),
			&D3DXVECTOR3(0.0f, 0.4f, 0.0f),
			&D3DXVECTOR3(0.0f, 1.0f, 0.0f));
		mDevice->SetTransform(D3DTS_PROJECTION, &mProjMat);
		mDevice->SetTransform(D3DTS_VIEW, &mViewMat);

		//Setup Direct3D
		mDevice->SetRenderState(D3DRS_LIGHTING, false);

		//Create scene light (only effective when cel shading is disabled)
		D3DLIGHT9 light = CreateDirectionalLight(D3DXVECTOR3(-0.1f, 0.5f, 1.0f), WHITE);
		mDevice->SetLight(0, &light);
		mDevice->LightEnable(0, TRUE);

		//--------------------------------------------
		//Create cel Vertex Shader and define handles
		//--------------------------------------------
		hr = mMesh[0].mesh->GetDeclaration(mVertexElements);
		if(FAILED(hr)) throw CException(TranslateHResult(hr));
		hr = mDevice->CreateVertexDeclaration(mVertexElements, &mVertexDeclaration);
		if(FAILED(hr))
		{
			throw CException(TranslateHResult(hr));
		}
		
		hr = D3DXCompileShaderFromFile(L"Shader/cel_shading.hlsl", 0, 0, "main",
			"vs_1_0", D3DXSHADER_DEBUG, &vshaderBuffer, &errorBuffer, 
			&appConstTable);
		if(FAILED(hr))
		{
			printf("Error: %s\n", errorBuffer->GetBufferPointer());
			throw CException(L"Error while compiling Vertex Shader.");
		}

		hr = mDevice->CreateVertexShader((DWORD*)vshaderBuffer->GetBufferPointer(),
			&appVertexShader);
		if(FAILED(hr))
		{
			throw CException(TranslateHResult(hr));
		}

		SAFERELEASE(vshaderBuffer);

		mWorldViewMatrixHandle			= appConstTable->GetConstantByName(0, "WorldViewMatrix");
		mWorldViewProjMatrixHandle		= appConstTable->GetConstantByName(0, "WorldViewProjMatrix");
		mLightDirectionHandle			= appConstTable->GetConstantByName(0, "LightDirection");
		mLightAmbientIntensityHandle	= appConstTable->GetConstantByName(0, "LightAmbientIntensity");
		mLightDiffuseIntensityHandle	= appConstTable->GetConstantByName(0, "LightDiffuseIntensity");
		mAmbientMtrlHandle				= appConstTable->GetConstantByName(0, "AmbientMtrl");
		mDiffuseMtrlHandle				= appConstTable->GetConstantByName(0, "DiffuseMtrl");
		if(!mWorldViewMatrixHandle || !mWorldViewProjMatrixHandle || !mLightDirectionHandle ||
			!mLightAmbientIntensityHandle || !mLightDiffuseIntensityHandle || !mAmbientMtrlHandle ||
			!mDiffuseMtrlHandle)
		{
			throw CException(L"Problem in cel_shading", LINE_AND_FILE);
		}
		appConstTable->SetVector(mDevice, mLightDirectionHandle, (D3DXVECTOR4*)&D3DXVECTOR3(-0.2f, 0.8f, 2.0f));
		D3DXVECTOR4 a(0.4f, 0.4f, 0.4f, 1.0f);
		appConstTable->SetVector(mDevice, mLightAmbientIntensityHandle, &a);
		appConstTable->SetVector(mDevice, mLightDiffuseIntensityHandle, (D3DXVECTOR4*)&WHITE);
		appConstTable->SetDefaults(mDevice);

		// Load cel texture
		hr = D3DXCreateTextureFromFile(mDevice, L"Textures/cel.png", &appTexture);
		if(FAILED(hr))
		{
			throw CException(TranslateHResult(hr));
		}
	}
}

void CApplication::destroy()
{
	for(UINT i = 0; i < 6; i++)
	{
		mMesh[i].Release();
	}
	SAFERELEASE(appVertexShader);
	SAFERELEASE(appConstTable);
	SAFERELEASE(mVertexDeclaration);
	SAFERELEASE(appTexture);
}

//Input system
//TO DO: refine: add mouse control
void CApplication::update(float timeDelta)
{
	static bool h_key_down = false, o_key_down = false, t_key_down = false, 
		num_key_down = false, w_key_down = false, d_key_down = false;
	if(!CApplication::isRunning)
	{
		return;
	}
	if(mDevice)
	{
		if(AsyncKey('A')){
			yaw += timeDelta;
		}
		if(AsyncKey('D')){
			yaw -= timeDelta;
		}
		if(AsyncKey('W')){
			pitch += timeDelta;
		}
		if(AsyncKey('S')){
			pitch -= timeDelta;
		}
		if(AsyncKey('R')){//reset view
			pitch = yaw = 0.0f;
		}
		if(AsyncKey(VK_UP)){//zoom in
			zoom += timeDelta * 5;
		}
		if(AsyncKey(VK_DOWN)){//zoom out
			zoom -= timeDelta * 5;
		}
		if(AsyncKey('H')){//show/hide help text
			if(!h_key_down){
				m_bShowHelp = !m_bShowHelp;
			}
			h_key_down = true;
		}
		else{
			h_key_down = false;
		}
		if(AsyncKey('O')){//enable/disable outline effect
			if(!o_key_down){
				m_bRenderOutlines = !m_bRenderOutlines;
			}
			o_key_down = true;
		}
		else{
			o_key_down = false;
		}
		if(AsyncKey('Y')){//enable cel effect
			if(!t_key_down){
				m_eShadingMode = E_cel_SHADING;
			}
			t_key_down = true;
		}
		else{
			t_key_down = false;
		}
		if(AsyncKey('U')){//enable diffuse shading
			if(!d_key_down){
				m_eShadingMode = E_DIFFUSE_SHADING;
			}
			d_key_down = true;
		}
		else{
			d_key_down = false;
		}
		if(AsyncKey('I')){//enable white shading
			if(!w_key_down){
				m_eShadingMode = E_WHITE_SHADING;
			}
			w_key_down = true;
		}
		else{
			w_key_down = false;
		}
		if(AsyncKey('1')){//car
			if(!num_key_down){
				m_uCurrentMesh = 0;
			}
			num_key_down = true;
		}
		else if(AsyncKey('2')){//teapot
			if(!num_key_down){
				m_uCurrentMesh = 1;
			}
			num_key_down = true;
		}
		else if(AsyncKey('3')){//dino
			if(!num_key_down){
				m_uCurrentMesh = 2;
			}
			num_key_down = true;
		}
		else if(AsyncKey('4')){//spaceship
			if(!num_key_down){
				m_uCurrentMesh = 3;
			}
			num_key_down = true;
		}
		else if(AsyncKey('5')){//cube
			if(!num_key_down){
				m_uCurrentMesh = 4;
			}
			num_key_down = true;
		}
		else if(AsyncKey('6')){//sphere
			if(!num_key_down){
				m_uCurrentMesh = 5;
			}
			num_key_down = true;
		}
		else{
			num_key_down = false;
		}
	}
}

void CApplication::render()
{
	if(mDevice)
	{
		D3DXMATRIX W, WV, M;
		D3DXMatrixLookAtLH(&mViewMat, 
			&D3DXVECTOR3(0.0f, 0.6f, zoom),
			&D3DXVECTOR3(0.0f, 0.4f, 0.0f),
			&D3DXVECTOR3(0.0f, 1.0f, 0.0f));
		D3DXMatrixRotationYawPitchRoll(&W, yaw, pitch, 0.0f);
		WV = W * mViewMat;
		appConstTable->SetMatrix(mDevice, mWorldViewMatrixHandle, &WV);
		M = WV * mProjMat;
		appConstTable->SetMatrix(mDevice, mWorldViewProjMatrixHandle, &M);

		mDevice->Clear(0, 0, D3DCLEAR_TARGET | D3DCLEAR_ZBUFFER, 0x00CCFFFF, 1.0f, 0);
		mDevice->BeginScene();
	
		mDevice->SetVertexDeclaration(mVertexDeclaration);
		if(m_eShadingMode == E_cel_SHADING)
		{
			//Disable lighting - required for cel shading effect
			mDevice->SetRenderState(D3DRS_LIGHTING, FALSE);
			mDevice->SetVertexShader(appVertexShader);
			mDevice->SetTexture(0, appTexture);
		}
		else
		{
			//White shading for testing
			if(m_eShadingMode == E_WHITE_SHADING)
			{
				mDevice->SetRenderState(D3DRS_LIGHTING, FALSE);
			}
			else
			{
				mDevice->SetRenderState(D3DRS_LIGHTING, TRUE);
			}
			mDevice->SetTransform(D3DTS_WORLD, &W);
			mDevice->SetTransform(D3DTS_VIEW, &mViewMat);
			mDevice->SetTransform(D3DTS_PROJECTION, &mProjMat);
			mDevice->SetTexture(0, NULL);
		}
	
		for(unsigned int i = 0; i < mMesh[m_uCurrentMesh].materialCount; i++)
		{
			if(m_eShadingMode == E_cel_SHADING)
			{
				appConstTable->SetVector(mDevice, mAmbientMtrlHandle, 
					(D3DXVECTOR4*)&mMesh[m_uCurrentMesh].materials[i].Ambient);
				appConstTable->SetVector(mDevice, mDiffuseMtrlHandle, 
					(D3DXVECTOR4*)&mMesh[m_uCurrentMesh].materials[i].Diffuse);
			}
			else
			{
				mDevice->SetMaterial(&mMesh[m_uCurrentMesh].materials[i]);
				mDevice->SetVertexShader(NULL);
			}
			mMesh[m_uCurrentMesh].mesh->DrawSubset(i);
		}

		if(m_bRenderOutlines)
		{
			mMesh[m_uCurrentMesh].MeshOutline->render(WV, mProjMat);
		}

		renderUI(0xff000000);

		mDevice->EndScene();
		mDevice->Present(0, 0, 0, 0);
	}
}

//simple GUI (help&fps)
void CApplication::renderUI(DWORD color)
{
	wchar_t buffer[16];
	swprintf(buffer, L"FPS: %d", (UINT)mWin->getFPS());
	mD3D->getFont()->DrawText(0.0f, (float)m_iHeight - 15.0f, color, buffer);

	//Help text
	if(m_bShowHelp)
	{
		mD3D->getFont()->DrawText(0.0f, 0.0f, 0xff000000, 
			L"Camera Controls:\n\n"
			L"-  WASD to rotate\n\n"
			L"-  Up/Down arrows zoom In/Out\n\n"
			L"-  R: Reset rotation\n\n\n"
			L"Model Controls:\n\n"
			L"-  1-6 Select mesh\n\n\n"
			L"Shading Controls:\n\n"
			L"-  Y: Cel Shading\n\n"
			L"-  U: Diffuse shading\n\n"
			L"-  I: White shading\n\n"
			L"-  O: Enable/Disable highlight outlines\n\n"
			L"H: Show/Hide help\n\n"
			);
	}
	else
	{
		mD3D->getFont()->DrawText(0.0f, 0.0f, 0xff000000,
			L"'H' for help");
	}
}

bool CApplication::faceIndex(UINT *face, UINT index) const
{
	UINT i = 0;
	for(i = 0; i < 3; i++)
	{
		if(face[i] == index)
		{
			return true;
		}
	}
	return false;
}