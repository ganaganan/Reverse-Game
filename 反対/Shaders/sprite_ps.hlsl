#include"sprite.hlsli"

Texture2D diffuse_map : register(t0);
SamplerState diffuse_map_sampler_state : register(s0);
float4 main(VS_OUT pin) : SV_TARGET
{
	return pin.color * diffuse_map.Sample(diffuse_map_sampler_state, pin.texcoord);
}