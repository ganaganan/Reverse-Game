#include "skinned_mesh.hlsli"

Texture2D    myTexture : register(t0); //�e�N�X�`���[
SamplerState mySampler : register(s0); //�T���v���[

#if 1
float4 main(VS_OUT pin) : SV_TARGET
{
	return myTexture.Sample(mySampler, pin.texcoord) * pin.color;
}

#else
float4 main(VS_OUT_POINT pin) : SV_TARGET
{
	// ��
	float3 dir;
	float len;
	float colD;
	float colA;
	float col;

	// �E
	float3 dirL;
	float lenL;
	float colDL;
	float colAL;
	float colL;


	// �_�����̕���
	dir = lightPosRight.xyz - pin.posw.xyz;

	// �_�����̋���
	len = length(dir);

	// �_�����̕�����normalize
	dir = dir / len;

	// �g�U
	float3 nor3 = pin.norw.xyz;
	colD = saturate(dot(nor3, dir));

	// ����
	colA = saturate(1.0f / (attenuation.x + attenuation.y * len + attenuation.z * len * len));

	col = colD * colA;
	return myTexture.Sample(mySampler, pin.texcoord) * pin.color * col;
}

#endif