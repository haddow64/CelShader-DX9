////////////////////////////////////////////////////////////////
// File: highlight_mesh.h
// Desc: Builds a highlight outline mesh from a given D3DXMESH and render it.
////////////////////////////////////////////////////////////////
#ifndef __HIGHLIGHT_MESH_H__
#define __HIGHLIGHT_MESH_H__

#include "stddef.h"
#include "framework/d3d.h"

const D3DVERTEXELEMENT9 VertexHighlightElements [] =
{
	{ 0, 0, D3DDECLTYPE_FLOAT3, D3DDECLMETHOD_DEFAULT, D3DDECLUSAGE_POSITION, 0 },
	{ 0, 12, D3DDECLTYPE_FLOAT3, D3DDECLMETHOD_DEFAULT, D3DDECLUSAGE_NORMAL, 0 },
	{ 0, 24, D3DDECLTYPE_FLOAT3, D3DDECLMETHOD_DEFAULT, D3DDECLUSAGE_NORMAL, 1 },
	{ 0, 36, D3DDECLTYPE_FLOAT3, D3DDECLMETHOD_DEFAULT, D3DDECLUSAGE_NORMAL, 2 },
	D3DDECL_END()
};

//Vertex Outline
//Position    = position of the vertex
//Normal 	  = vertex normal
//faceNormal = normal of face that the vertex is on
//faceNormalAdjacent = normal of the adjacent face
struct VertexHighlight
{
	VertexHighlight() { }
	VertexHighlight(const D3DXVECTOR3& _position, const D3DXVECTOR3& _normal,
		const D3DXVECTOR3& _faceNormal, const D3DXVECTOR3& _faceNormalAdjacent)
	{
		position = _position;
		normal = _normal;
		faceNormal = _faceNormal;
		faceNormalAdjacent = _faceNormalAdjacent;
	}

	D3DXVECTOR3 position;
	D3DXVECTOR3 normal;
	D3DXVECTOR3 faceNormal;
	D3DXVECTOR3 faceNormalAdjacent;
};

//Intermediate Mesh Vertex
//Used to read d3d mesh vertices in the specific format needed.
struct MeshVertex
{
	D3DXVECTOR3 position;
	D3DXVECTOR3 normal;
	static const DWORD FVF;
};

class CMeshOutline
{
public:
	CMeshOutline(LPDIRECT3DDEVICE9 d3dDevice, LPD3DXMESH d3dMesh) :
	  mDevice(d3dDevice)
	{
		assert(mDevice);
		printf("<<<<<<>>>>>>\nCMeshOutline\n<<<<<<>>>>>>\n");
		printf("Mesh Details:\n");
		printf("vertex count: %d\n", d3dMesh->GetNumVertices());
		printf("face count: %d\n", d3dMesh->GetNumFaces());
		printf("index count: %d\n", d3dMesh->GetNumFaces() * 3);
		mFaceCount = d3dMesh->GetNumFaces() * 6;
		mVertexCount = d3dMesh->GetNumFaces() * 12;
		
		HRESULT hr;
		UINT adjacentFaceCount = 0;

		//Outlines vertex buffer
		hr = mDevice->CreateVertexBuffer(sizeof(VertexHighlight) * mVertexCount,
			D3DUSAGE_WRITEONLY, 0, D3DPOOL_MANAGED, &mVB, 0);
		if(FAILED(hr)) throw CException(TranslateHResult(hr), LINE_AND_FILE);
		//Outlines index buffer
		hr = mDevice->CreateIndexBuffer(sizeof(UINT) * mFaceCount * 3,
			D3DUSAGE_WRITEONLY, D3DFMT_INDEX32, D3DPOOL_MANAGED, &mIB, 0);
		if(FAILED(hr)) throw CException(TranslateHResult(hr), LINE_AND_FILE);
		//Outlines vertex declaration
		hr = mDevice->CreateVertexDeclaration(VertexHighlightElements, &mDeclaration);
		if(FAILED(hr)) throw CException(TranslateHResult(hr), LINE_AND_FILE);

		LPD3DXMESH tempMesh = NULL;
		DWORD* adjData = new DWORD [d3dMesh->GetNumFaces() * 3];
		hr = d3dMesh->CloneMeshFVF(D3DXMESH_SYSTEMMEM | D3DXMESH_32BIT, 
			(D3DFVF_XYZ | D3DFVF_NORMAL), mDevice, &tempMesh);
		if(FAILED(hr)) throw CException(TranslateHResult(hr), LINE_AND_FILE);
		
		UINT *meshIndices = NULL, *outlineIndices = NULL;
		MeshVertex *meshVertices = NULL;
		VertexHighlight *outlineVertices = NULL;

		hr = mVB->Lock(0, 0, (void**)&outlineVertices, D3DLOCK_DISCARD);
		if(FAILED(hr)) throw CException(TranslateHResult(hr), LINE_AND_FILE);
		hr = mIB->Lock(0, 0, (void**)&outlineIndices, D3DLOCK_DISCARD);
		if(FAILED(hr)) throw CException(TranslateHResult(hr), LINE_AND_FILE);
		hr = tempMesh->LockVertexBuffer(0, (void**)&meshVertices);
		if(FAILED(hr)) throw CException(TranslateHResult(hr), LINE_AND_FILE);
		hr = tempMesh->LockIndexBuffer(0, (void**)&meshIndices);
		if(FAILED(hr)) throw CException(TranslateHResult(hr), LINE_AND_FILE);
		hr = tempMesh->GenerateAdjacency(0.001f, adjData);
		if(FAILED(hr)) throw CException(TranslateHResult(hr), LINE_AND_FILE);

		D3DXVECTOR3 faceNormal, faceNormalAdjacent;
		D3DXVECTOR3 v[3], n[3], t0, t1;
		DWORD a[3];
		UINT indx[3];
		UINT cv = 0;
		UINT ci = 0;

		//For each mesh face
		for(UINT i = 0; i < tempMesh->GetNumFaces(); i++)
		{
			indx[0] = meshIndices[i * 3 + 0];
			indx[1] = meshIndices[i * 3 + 1];
			indx[2] = meshIndices[i * 3 + 2];
			v[0] = meshVertices[indx[0]].position;
			v[1] = meshVertices[indx[1]].position;
			v[2] = meshVertices[indx[2]].position;
			n[0] = meshVertices[indx[0]].normal;
			n[1] = meshVertices[indx[1]].normal;
			n[2] = meshVertices[indx[2]].normal;

			//Calculate the current face normal
			t0 = v[1] - v[0];
			t1 = v[2] - v[0];
			D3DXVec3Cross(&faceNormal, &t1, &t0);
			D3DXVec3Normalize(&faceNormal, &faceNormal);
			
			//What faces are adjacent to this one?
			a[0] = adjData[i * 3 + 0];
			a[1] = adjData[i * 3 + 1];
			a[2] = adjData[i * 3 + 2];

			//For each edge, build outline
			UINT j = 0;
			for(UINT e = 0; e < 3; e++)
			{
				j = (e + 1) % 3; //e/j = 0/1 - 1/2 - 2/0
				outlineVertices[cv + 0].position = v[e];
				outlineVertices[cv + 1].position = v[e];
				outlineVertices[cv + 2].position = v[j];
				outlineVertices[cv + 3].position = v[j];

				outlineVertices[cv + 0].normal	 = D3DXVECTOR3(0.0f, 0.0f, 0.0f);
				outlineVertices[cv + 1].normal   = n[e];
				outlineVertices[cv + 2].normal   = D3DXVECTOR3(0.0f, 0.0f, 0.0f);
				outlineVertices[cv + 3].normal   = n[j];

				//Get adjacent face for this edge
				faceNormalAdjacent = -faceNormal;
				for(UINT k = 0; k < 3; k++)
				{
					if(a[k] != ULONG_MAX)
					{
						UINT* d = &meshIndices[a[k] * 3];
						if(faceIndex(d, indx[e]) &&
						   faceIndex(d, indx[j]))
						{
							//This is the face that shares edge
							t0 = meshVertices[d[1]].position - meshVertices[d[0]].position;
							t1 = meshVertices[d[2]].position - meshVertices[d[0]].position;
							D3DXVec3Cross(&faceNormalAdjacent, &t1, &t0);
							D3DXVec3Normalize(&faceNormalAdjacent, &faceNormalAdjacent);
							adjacentFaceCount++;
							break;
						}
					}
				}
				outlineVertices[cv + 0].faceNormal = faceNormal;
				outlineVertices[cv + 1].faceNormal = faceNormal;
				outlineVertices[cv + 2].faceNormal = faceNormal;
				outlineVertices[cv + 3].faceNormal = faceNormal;
				outlineVertices[cv + 0].faceNormalAdjacent = faceNormalAdjacent;
				outlineVertices[cv + 1].faceNormalAdjacent = faceNormalAdjacent;
				outlineVertices[cv + 2].faceNormalAdjacent = faceNormalAdjacent;
				outlineVertices[cv + 3].faceNormalAdjacent = faceNormalAdjacent;

				outlineIndices[ci + 0] = cv + 0;
				outlineIndices[ci + 1] = cv + 1;
				outlineIndices[ci + 2] = cv + 3;
				outlineIndices[ci + 3] = cv + 0;
				outlineIndices[ci + 4] = cv + 3;
				outlineIndices[ci + 5] = cv + 2;

				cv += 4;
				ci += 6;
			}
		}

		printf("Written vertices: %d\n", cv);
		printf("Written indices: %d\n", ci);
		printf("Number of adjacent face normals: %d\n", adjacentFaceCount);

		tempMesh->UnlockVertexBuffer();
		tempMesh->UnlockIndexBuffer();
		mIB->Unlock();
		mVB->Unlock();
		SAFEDELETE(adjData);
		SAFERELEASE(tempMesh);

		//Create Highlight Vertex Shader and define handles
		LPD3DXBUFFER vshaderBuffer, errorBuffer;

		hr = D3DXCompileShaderFromFile(L"Shader/highlight_edges.hlsl",
			NULL, NULL, "main", "vs_1_0", D3DXSHADER_DEBUG, &vshaderBuffer, &errorBuffer, 
			&mConstantTable);
		if(FAILED(hr))
		{
			printf("Error: %s\n", errorBuffer->GetBufferPointer());
			throw CException(L"Error while compiling Vertex Shader.");
		}

		hr = mDevice->CreateVertexShader((DWORD*)vshaderBuffer->GetBufferPointer(),
			&mVertexShader);
		if(FAILED(hr)) throw CException((const char*)errorBuffer->GetBufferPointer(), LINE_AND_FILE);

		SAFERELEASE(vshaderBuffer);

		mWorldViewMatrixHandle = mConstantTable->GetConstantByName(0, "WorldViewMatrix");
		mProjMatrixHandle = mConstantTable->GetConstantByName(0, "ProjMatrix");
		if(!mWorldViewMatrixHandle || !mProjMatrixHandle)
		{
			throw CException(L"Something went wrong in highlight_edges",
				LINE_AND_FILE);
		}
		mConstantTable->SetDefaults(mDevice);
	}

	virtual ~CMeshOutline()
	{
		SAFERELEASE(mConstantTable);
		SAFERELEASE(mVertexShader);
		SAFERELEASE(mDeclaration);
		SAFERELEASE(mVB);
		SAFERELEASE(mIB);
	}

	void render(D3DXMATRIX &worldViewMatrix, D3DXMATRIX &projMatrix)
	{
		mDevice->SetRenderState(D3DRS_LIGHTING, FALSE);
		mDevice->SetRenderState(D3DRS_CULLMODE, D3DCULL_NONE);
		mDevice->SetVertexDeclaration(mDeclaration);
		mDevice->SetStreamSource(0, mVB, 0, sizeof(VertexHighlight));
		mDevice->SetIndices(mIB);
		mDevice->SetTexture(0, 0);
		mDevice->SetVertexShader(mVertexShader);
		mConstantTable->SetMatrix(mDevice, mWorldViewMatrixHandle, &worldViewMatrix);
		mConstantTable->SetMatrix(mDevice, mProjMatrixHandle, &projMatrix);
		mDevice->DrawIndexedPrimitive(D3DPT_TRIANGLELIST, 0, 0, mVertexCount,
			0, mFaceCount);
		mDevice->SetRenderState(D3DRS_CULLMODE, D3DCULL_CCW);
	}

private:
	bool faceIndex(UINT* face, UINT i1)
	{
		for(int i = 0; i < 3; i++)
		{
			if(face[i] == i1)
			{
				return true;
			}
		}
		return false;
	}

	LPDIRECT3DDEVICE9				mDevice;
	LPDIRECT3DVERTEXDECLARATION9	mDeclaration;
	LPDIRECT3DVERTEXBUFFER9			mVB;
	LPDIRECT3DINDEXBUFFER9			mIB;

	LPDIRECT3DVERTEXSHADER9			mVertexShader;
	LPD3DXCONSTANTTABLE				mConstantTable;
	D3DXHANDLE						mWorldViewMatrixHandle;
	D3DXHANDLE						mProjMatrixHandle;

	UINT							mVertexCount;
	UINT							mFaceCount;
};

#endif