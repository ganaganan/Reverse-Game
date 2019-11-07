#include "skinned_mesh.hlsli"

Texture2D    myTexture : register(t0); //�e�N�X�`���[
SamplerState mySampler : register(s0); //�T���v���[

// �g�U���ˊ֐�
float3 Diffuse(float3 N, float3 L, float3 C, float3 K)
{
	float D = dot(N, L);
	D = max(0, D);
	return K * C* D;
}

float3 BlinnPhongSpecular(float3 N, float3 L, float3 C,
	float3 E, float3 K, float power)
{
	// �n�[�t�x�N�g��
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
	// ���C�g�x�N�g��
	float3 L = normalize(-light_direction.xyz);
	// �@���i���[���h�j
	float3 N = pin.norw.xyz;
	// ���C�g�J���[
	float3 C = lightColor.rgb;
	// �J��������_�ւ̃x�N�g��
	float3 E = normalize(eyePos.xyz - pin.posw.xyz);
	// ����
	float3 A = ambientColor.rgb;
	// �g�U���ˌ�
	float3 kd = float3(1, 1, 1);
	float3 D = Diffuse(N, L, C, kd);
	//	return float4(D, 1.0f);
		// ���ʔ��ˌ�
		float3 ks = float3(1, 1, 1);
		float3 S = BlinnPhongSpecular(N, L, C, E, ks, 20);

		// �|�C���g���C�g
		float3 PL;
		float3 PC;
		float influence;
		float3 PD = (float3)0;
		float3 PS = (float3)0;
		[unroll(POINTMAX)] for (int i = 0; i < POINTMAX; ++i)
		{
			// �_�����Ă��Ȃ����C�g���O
			if (pointLight[i].type == 0)continue;
			PL = pin.posw.xyz - pointLight[i].pos.xyz;
			float d = length(PL);

			float r = pointLight[i].range;
			// �����͂��Ȃ����C�g���O
			if (d > r)continue;

			influence = saturate(1.0f - d / r);
			PL = normalize(PL);
			PC = pointLight[i].color.rgb;
			// �g�U���̉��Z
			PD += Diffuse(N, PL, PC, kd) * influence * influence;
			// ���ʌ��̉��Z
			PS += BlinnPhongSpecular(N, PL, PC, E, ks, 20) * influence * influence;
		}

		float4 color = myTexture.Sample(mySampler, pin.texcoord);
		return color * float4(PD, 1.0f);

		return color * pin.color * float4(A + D + S + PD + PS, 1.0f);
}

#endif