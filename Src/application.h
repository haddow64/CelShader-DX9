////////////////////////////////////////////////////////////////
//File: application.h
//Desc: Application class
////////////////////////////////////////////////////////////////
#ifndef __APPLICATION_H__
#define __APPLICATION_H__

#include "stddef.h"
#include "framework/d3d.h"
#include "highlight_mesh.h"

struct XMeshCarcel
{
	XMeshCarcel() : mesh(NULL), materials(NULL), MeshOutline(NULL) {}
	XMeshCarcel(LPDIRECT3DDEVICE9 d3dDevice, const wchar_t* meshFile)
	{
		assert(d3dDevice);
		HRESULT hr;
		D3DXMATERIAL* mtrls = NULL;
		LPD3DXBUFFER materialBuffer = NULL;

		//Load X mesh
		hr = D3DXLoadMeshFromX(meshFile, D3DXMESH_SYSTEMMEM,
			d3dDevice, NULL, &materialBuffer, NULL,
			&materialCount, &mesh);
		if(FAILED(hr))
		{
			std::wstring strErr = L"Problem loading mesh: \"";
			strErr.append(meshFile);
			strErr.append(L"\"");
			throw CException(strErr, LINE_AND_FILE);
		}
		mtrls = (D3DXMATERIAL*)materialBuffer->GetBufferPointer();
		materials = new D3DMATERIAL9 [materialCount];
		for(UINT i = 0; i < materialCount; i++)
		{
			materials[i] = mtrls[i].MatD3D;
			materials[i].Ambient = materials[i].Diffuse;
		}
		SAFERELEASE(materialBuffer);

		//Create highlight mesh
		MeshOutline = new CMeshOutline(d3dDevice, mesh);
	}

	void Release()
	{
		SAFEDELETE(MeshOutline);
		SAFEDELETE(materials);
		SAFERELEASE(mesh);
	}

	LPD3DXMESH						mesh;
	DWORD							materialCount;
	D3DMATERIAL9*					materials;
	CMeshOutline*					MeshOutline;
};

enum EShadingMode
{
	E_DIFFUSE_SHADING = 0,
	E_cel_SHADING,
	E_WHITE_SHADING,
};

class CApplication
{
public:
	CApplication();
	CApplication(const wchar_t* applicationTitle);
	CApplication(const wchar_t* applicationTitle, int width, int height, bool fullscreen);
	virtual ~CApplication();
	
	void start();
	void update(float timeDelta);
	void render();
	void renderUI(DWORD color = 0xffffffff);
	
	int getWidth() const { return m_iWidth; }
	int getHeight() const { return m_iHeight; }
	bool isFullscreen() const { return m_bFullscreen; }
	const wchar_t* getTitle() const { return m_strAppName.c_str(); }

	static bool isRunning;

private:
	//Application initialization methods
	void create();
	void destroy();

	bool faceIndex(UINT* face, UINT index) const;

	//Window options
	int					m_iWidth;
	int					m_iHeight;
	bool				m_bFullscreen;

	//Handles/Objects
	CWindows*			mWin;
	CD3D*				mD3D;
	IDirect3DDevice9*	mDevice;
	
	//Strings
	std::wstring		m_strAppName;

	//Application specific
	LPDIRECT3DTEXTURE9				appTexture;
	LPDIRECT3DVERTEXSHADER9			appVertexShader;
	LPD3DXCONSTANTTABLE				appConstTable;
	D3DVERTEXELEMENT9				mVertexElements [MAX_FVF_DECL_SIZE];
	LPDIRECT3DVERTEXDECLARATION9	mVertexDeclaration;
	D3DXHANDLE						mWorldViewMatrixHandle;
	D3DXHANDLE						mWorldViewProjMatrixHandle;
	D3DXHANDLE						mLightDirectionHandle;
	D3DXHANDLE						mLightAmbientIntensityHandle;
	D3DXHANDLE						mLightDiffuseIntensityHandle;
	D3DXHANDLE						mAmbientMtrlHandle;
	D3DXHANDLE						mDiffuseMtrlHandle;
	XMeshCarcel						mMesh [6];
	D3DXMATRIX						mViewMat;
	D3DXMATRIX						mProjMat;

	//Parameters
	UINT				m_uCurrentMesh;
	bool				m_bShowHelp;
	bool				m_bRenderOutlines;
	EShadingMode		m_eShadingMode;
};

#endif