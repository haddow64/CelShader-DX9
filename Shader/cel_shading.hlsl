///////////////////////////////////////////////////////////////
// Desc: cel Shading Vertex Shader
///////////////////////////////////////////////////////////////

matrix WorldViewMatrix;
matrix WorldViewProjMatrix;
vector LightDirection;
vector LightAmbientIntensity;
vector LightDiffuseIntensity;
vector AmbientMtrl;
vector DiffuseMtrl;

struct VS_INPUT
{
	vector position : POSITION;
	vector normal : NORMAL;
};

struct VS_OUTPUT
{
	vector position : POSITION;
	float2 uv : TEXCOORD0;
	vector color : COLOR0;
};

VS_OUTPUT main(VS_INPUT IN)
{
	VS_OUTPUT OUT;
	IN.position.w = 1.0f;
	IN.normal.w = 0.0f;
	OUT.position = mul(IN.position, WorldViewProjMatrix);
	IN.normal = mul(IN.normal, WorldViewMatrix);
	float s = max(dot(-IN.normal, LightDirection), 0.0f);
	OUT.uv = float2(s, 0.5f);
	OUT.color = (AmbientMtrl * LightAmbientIntensity) + (s * (LightDiffuseIntensity * DiffuseMtrl));
	
	return OUT;
}