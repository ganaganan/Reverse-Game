#include "skinned_mesh.hlsli"

Texture2D    myTexture : register(t0); //テクスチャー
SamplerState mySampler : register(s0); //サンプラー

#if 1
float4 main(VS_OUT pin) : SV_TARGET
{
	return myTexture.Sample(mySampler, pin.texcoord) * pin.color;
}

#else
float4 main(VS_OUT_POINT pin) : SV_TARGET
{
	// 左
	float3 dir;
	float len;
	float colD;
	float colA;
	float col;

	// 右
	float3 dirL;
	float lenL;
	float colDL;
	float colAL;
	float colL;


	// 点光源の方向
	dir = lightPosRight.xyz - pin.posw.xyz;

	// 点光源の距離
	len = length(dir);

	// 点光源の方向をnormalize
	dir = dir / len;

	// 拡散
	float3 nor3 = pin.norw.xyz;
	colD = saturate(dot(nor3, dir));

	// 減衰
	colA = saturate(1.0f / (attenuation.x + attenuation.y * len + attenuation.z * len * len));

	col = colD * colA;
	return myTexture.Sample(mySampler, pin.texcoord) * pin.color * col;
}

#endif