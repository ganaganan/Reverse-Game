#include "skinned_mesh.hlsli"

Texture2D    myTexture : register(t0); //テクスチャー
SamplerState mySampler : register(s0); //サンプラー

// 拡散反射関数
float3 Diffuse(float3 N, float3 L, float3 C, float3 K)
{
	float D = dot(N, L);
	D = max(0, D);
	return K * C* D;
}

float3 BlinnPhongSpecular(float3 N, float3 L, float3 C,
	float3 E, float3 K, float power)
{
	// ハーフベクトル
	float3 H = normalize(-L + E);

	float3 S = dot(H, N);

	S = max(0, S);
	S = pow(S, power);
	S = S * K * C;
	return S;
}

#if 0
float4 main(VS_OUT pin) : SV_TARGET
{
	return myTexture.Sample(mySampler, pin.texcoord) * pin.color;
}

#else
float4 main(VS_OUT_POINT pin) : SV_TARGET
{
	// ライトベクトル
	float3 L = normalize(-light_direction.xyz);
	// 法線（ワールド）
	float3 N = pin.norw.xyz;
	// ライトカラー
	float3 C = lightColor.rgb;
	// カメラから点へのベクトル
	float3 E = normalize(eyePos.xyz - pin.posw.xyz);
	// 環境光
	float3 A = ambientColor.rgb;
	// 拡散反射光
	float3 kd = float3(1, 1, 1);
	float3 D = Diffuse(N, L, C, kd);
	//	return float4(D, 1.0f);
		// 鏡面反射光
		float3 ks = float3(1, 1, 1);
		float3 S = BlinnPhongSpecular(N, L, C, E, ks, 20);

		// ポイントライト
		float3 PL;
		float3 PC;
		float influence;
		float3 PD = (float3)0;
		float3 PS = (float3)0;
		[unroll(POINTMAX)] for (int i = 0; i < POINTMAX; ++i)
		{
			// 点灯していないライト除外
			if (pointLight[i].type == 0)continue;
			PL = pin.posw.xyz - pointLight[i].pos.xyz;
			float d = length(PL);

			float r = pointLight[i].range;
			// 光が届かないライト除外
			if (d > r)continue;

			influence = saturate(1.0f - d / r);
			PL = normalize(PL);
			PC = pointLight[i].color.rgb;
			// 拡散光の加算
			PD += Diffuse(N, PL, PC, kd) * influence * influence;
			// 鏡面光の加算
			PS += BlinnPhongSpecular(N, PL, PC, E, ks, 20) * influence * influence;
		}

		float4 color = myTexture.Sample(mySampler, pin.texcoord);
		return color * float4(PD, 1.0f);

		return color * pin.color * float4(A + D + S + PD + PS, 1.0f);
}

#endif