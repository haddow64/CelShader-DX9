///////////////////////////////////////////////////////////////
// Desc: Highlighted Edges drawing vertex shader
///////////////////////////////////////////////////////////////

matrix WorldViewMatrix;
matrix ProjMatrix;

struct VS_INPUT
{
	vector position : POSITION;
	vector normal : NORMAL0;
	vector faceNormal : NORMAL1;
	vector faceNormalAdjacent : NORMAL2;
};

struct VS_OUTPUT
{
	vector position : POSITION;
	vector color : COLOR0;
};

VS_OUTPUT main(VS_INPUT IN)
{
	VS_OUTPUT OUT;
	
	IN.position = mul(IN.position, WorldViewMatrix);
	vector eyeVector = IN.position;
	
	IN.normal.w = 0.0f;
	IN.faceNormal.w = 0.0f;
	IN.faceNormalAdjacent.w = 0.0f;
	
	IN.normal = mul(IN.normal, WorldViewMatrix);
	IN.faceNormal = mul(IN.faceNormal, WorldViewMatrix);
	IN.faceNormalAdjacent = mul(IN.faceNormalAdjacent, WorldViewMatrix);
	
	float a1 = dot(eyeVector, IN.faceNormal);
	float a2 = dot(eyeVector, IN.faceNormalAdjacent);
	float s = a1 * a2;
	if(s < 0)
	{
		IN.position += 0.05f * IN.normal;
	}
	
	OUT.position = mul(IN.position, ProjMatrix);
	OUT.color = vector(0.0f, 0.0f, 0.0f, 0.0f);
	
	return OUT;
}